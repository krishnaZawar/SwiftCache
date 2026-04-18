#include<thread>
#include<chrono>
#include<fstream>
#include<cstdio>

#include "test_utils.h"
#include "../src/base/errors.h"

using std::ofstream;
using std::ifstream;
using std::getline;
using std::this_thread::sleep_for;
using std::chrono::seconds;

namespace {
	void writeWal(const string &walFile, const vector<string> &logs) {
		ofstream file(walFile, std::ios::trunc);
		for (const auto &log : logs) {
			file << log << "\n";
		}
	}

	void appendWal(const string &walFile, const vector<string> &logs) {
		ofstream file(walFile, std::ios::app);
		for (const auto &log : logs) {
			file << log << "\n";
		}
	}

	void replayWal(const string &walFile, Parser &parser) {
		ifstream file(walFile);
		if (!file) {
			return;
		}
		string walLog;
		while (getline(file, walLog)) {
			try {
				parser.parseCommand(walLog.c_str(), walLog.size(), true);
			} catch (const string &err) {
				(void)err;
				continue;
			}
		}
	}

	vector<string> readWalLines(const string &walFile) {
		vector<string> lines;
		ifstream file(walFile);
		if (!file) {
			return lines;
		}
		string line;
		while (getline(file, line)) {
			if (!line.empty()) {
				lines.push_back(line);
			}
		}
		return lines;
	}

	bool containsSubstring(const vector<string> &lines, const string &needle) {
		for (const auto &line : lines) {
			if (line.find(needle) != string::npos) {
				return true;
			}
		}
		return false;
	}

	bool waitForCompactionCycle(WAL *wal, Parser &parser, const string &heartbeatKey) {
		string command;
		bool sawCompaction = false;
		for (int i = 0; i < 40; i++) {
			if (wal->isUnderCompaction()) {
				sawCompaction = true;
				command = "TYPE " + heartbeatKey;
				executeCommand(parser, command);
			}
			if (sawCompaction && !wal->isUnderCompaction()) {
				return true;
			}
			sleep_for(std::chrono::milliseconds(150));
		}
		return false;
	}

	void runScenarioPersistenceMixedReplay() {
		string command;

		const string walFile = "test/wal_persistence_test_dump_1.txt";
		writeWal(walFile, {
			"SET user:1 Alice",
			"SET user:1 Alicia",
			"RPUSH queue jobs-1 jobs-2",
			"LPUSH queue jobs-0",
			"HSET profile city delhi role engineer",
			"DEL stale",
			"SET live yes"
		});

		Database db;
		Parser parser(&db);
		replayWal(walFile, parser);

		command = "GET user:1";
		AssertEqual("WAL flow 1", executeCommand(parser, command), "Alicia");

		command = "LRANGE queue 0 2";
		AssertEqual("WAL flow 2", executeCommand(parser, command), "jobs-0\njobs-1\njobs-2");

		command = "HGET profile city role";
		AssertEqual("WAL flow 3", executeCommand(parser, command), "delhi\nengineer");

		std::remove(walFile.c_str());
	}

	void runScenarioPersistenceTTLAndPersist() {
		string command;

		const string walFile = "test/wal_persistence_test_dump_2.txt";
		writeWal(walFile, {
			"SET session token123",
			"EXPIRE session 1",
			"SET account premium",
			"EXPIRE account 1",
			"PERSIST account"
		});

		Database db;
		Parser parser(&db);
		replayWal(walFile, parser);

		command = "GET session";
		AssertEqual("WAL flow 4", executeCommand(parser, command), "token123");

		sleep_for(seconds(2));
		command = "GET session";
		AssertEqual("WAL flow 5", executeCommand(parser, command), ERR_KEY_NOT_FOUND + "\nError fetching session");

		command = "GET account";
		AssertEqual("WAL flow 6", executeCommand(parser, command), "premium");

		std::remove(walFile.c_str());
	}

	void runScenarioPersistenceMultiRestart() {
		string command;

		const string walFile = "test/wal_persistence_test_dump_3.txt";
		writeWal(walFile, {
			"SET cart:status open",
			"RPUSH cart:items apple banana carrot",
			"HSET cart:meta owner user1 city mumbai"
		});
		appendWal(walFile, {
			"LPOP cart:items 1",
			"LPUSH cart:items kiwi",
			"HDEL cart:meta city",
			"SET cart:status checked_out"
		});
		appendWal(walFile, {
			"DEL cart:items",
			"RPUSH cart:items final1 final2",
			"DEL cart:meta",
			"HSET cart:meta owner user2 state ready"
		});

		Database db;
		Parser parser(&db);
		replayWal(walFile, parser);

		command = "GET cart:status";
		AssertEqual("WAL flow 7", executeCommand(parser, command), "checked_out");

		command = "LRANGE cart:items 0 1";
		AssertEqual("WAL flow 8", executeCommand(parser, command), "final1\nfinal2");

		command = "HGET cart:meta owner state";
		AssertEqual("WAL flow 9", executeCommand(parser, command), "user2\nready");

		std::remove(walFile.c_str());
	}

	void runScenarioPersistenceLongMixed() {
		string command;

		const string walFile = "test/wal_persistence_test_dump_4.txt";
		writeWal(walFile, {
			"DEL long:list long:hash",
			"RPUSH long:list L1 L2",
			"LPUSH long:list L0",
			"SET long:s1 v1",
			"SET long:s2 v2",
			"HSET long:hash f0 h0 f1 h1",
			"HDEL long:hash f1",
			"HSET long:hash f3 h3",
			"RPUSH long:list L3",
			"SET long:s1 v1b",
			"LPUSH long:list Lstart",
			"LPOP long:list 1",
			"RPUSH long:list L4"
		});

		Database db;
		Parser parser(&db);
		replayWal(walFile, parser);

		command = "LLEN long:list";
		AssertEqual("WAL flow 10", executeCommand(parser, command), "5");

		command = "TYPE long:s1 long:list long:hash";
		AssertEqual("WAL flow 11", executeCommand(parser, command), "String List Hash");

		command = "HEXISTS long:hash f0 f1 f3";
		AssertEqual("WAL flow 12", executeCommand(parser, command), "101");

		std::remove(walFile.c_str());
	}

	void runScenarioCompactionActiveSnapshot() {
		string command;

		const string walFile = "test/wal_persistence_test_dump_12.txt";
		const string compactionWalFile = "test/wal_persistence_test_dump_12_compact.txt";
		std::remove(walFile.c_str());
		std::remove(compactionWalFile.c_str());

		WAL *wal = new WAL(walFile, 1, 1);
		Database db;
		Parser parser(&db, wal);
		db.addWAL(wal);

		command = "SET compact:s v1";
		executeCommand(parser, command);
		command = "SET compact:s final";
		executeCommand(parser, command);
		command = "RPUSH compact:list 1 2 3";
		executeCommand(parser, command);
		command = "LPOP compact:list 1";
		executeCommand(parser, command);
		command = "LPUSH compact:list 0";
		executeCommand(parser, command);
		command = "HSET compact:hash f1 a f2 b";
		executeCommand(parser, command);
		command = "HDEL compact:hash f1";
		executeCommand(parser, command);
		command = "SET compact:tmp x";
		executeCommand(parser, command);
		command = "DEL compact:tmp";
		executeCommand(parser, command);

		sleep_for(seconds(2));
		bool finishedCompaction = waitForCompactionCycle(wal, parser, "compact:s");
		sleep_for(seconds(1));

		vector<string> compactedLines = readWalLines(walFile);
		AssertEqual("WAL flow 13", finishedCompaction ? "1" : "0", "1");
		AssertEqual("WAL flow 14", (containsSubstring(compactedLines, "SET compact:s final") && !containsSubstring(compactedLines, "compact:tmp")) ? "1" : "0", "1");

		delete wal;

		Database replayDb;
		Parser replayParser(&replayDb);
		replayWal(walFile, replayParser);
		command = "GET compact:s";
		AssertEqual("WAL flow 15", executeCommand(replayParser, command), "final");

		std::remove(walFile.c_str());
		std::remove(compactionWalFile.c_str());
	}

	void runScenarioCompactionTempFileCleanup() {
		string command;

		const string walFile = "test/wal_persistence_test_dump_13.txt";
		const string compactionWalFile = "test/wal_persistence_test_dump_13_compact.txt";
		std::remove(walFile.c_str());
		std::remove(compactionWalFile.c_str());

		WAL *wal = new WAL(walFile, 1, 1);
		Database db;
		Parser parser(&db, wal);
		db.addWAL(wal);

		command = "SET cleanup:s ok";
		executeCommand(parser, command);
		command = "SET cleanup:dead tmp";
		executeCommand(parser, command);
		command = "DEL cleanup:dead";
		executeCommand(parser, command);

		sleep_for(seconds(2));
		bool finishedCompaction = waitForCompactionCycle(wal, parser, "cleanup:s");
		sleep_for(seconds(1));

		delete wal;
		bool compactTempExists = ifstream(compactionWalFile).good();

		AssertEqual("WAL flow 16", finishedCompaction ? "1" : "0", "1");
		AssertEqual("WAL flow 17", compactTempExists ? "1" : "0", "0");

		std::remove(walFile.c_str());
		std::remove(compactionWalFile.c_str());
	}

	void runScenarioCompactionMultipleCycles() {
		string command;

		const string walFile = "test/wal_persistence_test_dump_14.txt";
		const string compactionWalFile = "test/wal_persistence_test_dump_14_compact.txt";
		std::remove(walFile.c_str());
		std::remove(compactionWalFile.c_str());

		WAL *wal = new WAL(walFile, 1, 1);
		Database db;
		Parser parser(&db, wal);
		db.addWAL(wal);

		command = "SET cycle:s v1";
		executeCommand(parser, command);
		command = "RPUSH cycle:l 1 2";
		executeCommand(parser, command);
		command = "HSET cycle:h a one";
		executeCommand(parser, command);
		sleep_for(seconds(2));
		bool firstCycle = waitForCompactionCycle(wal, parser, "cycle:s");

		command = "SET cycle:s v2";
		executeCommand(parser, command);
		command = "LPOP cycle:l 1";
		executeCommand(parser, command);
		command = "LPUSH cycle:l 0";
		executeCommand(parser, command);
		command = "HSET cycle:h b two";
		executeCommand(parser, command);
		command = "HDEL cycle:h a";
		executeCommand(parser, command);

		sleep_for(seconds(2));
		bool secondCycle = waitForCompactionCycle(wal, parser, "cycle:s");
		sleep_for(seconds(1));

		vector<string> compactedLines = readWalLines(walFile);
		AssertEqual("WAL flow 18", (firstCycle && secondCycle) ? "1" : "0", "1");
		AssertEqual("WAL flow 19", (containsSubstring(compactedLines, "SET cycle:s v2") && !containsSubstring(compactedLines, "SET cycle:s v1")) ? "1" : "0", "1");

		delete wal;
		std::remove(walFile.c_str());
		std::remove(compactionWalFile.c_str());
	}

	void runScenarioCompactionReplayIntegrity() {
		string command;

		const string walFile = "test/wal_persistence_test_dump_15.txt";
		const string compactionWalFile = "test/wal_persistence_test_dump_15_compact.txt";
		std::remove(walFile.c_str());
		std::remove(compactionWalFile.c_str());

		WAL *wal = new WAL(walFile, 1, 1);
		Database db;
		Parser parser(&db, wal);
		db.addWAL(wal);

		command = "SET ri:s v1";
		executeCommand(parser, command);
		command = "SET ri:s v2";
		executeCommand(parser, command);
		command = "RPUSH ri:l a b";
		executeCommand(parser, command);
		command = "LPOP ri:l 1";
		executeCommand(parser, command);
		command = "LPUSH ri:l z";
		executeCommand(parser, command);
		command = "HSET ri:h f1 x f2 y";
		executeCommand(parser, command);
		command = "HDEL ri:h f1";
		executeCommand(parser, command);

		sleep_for(seconds(2));
		waitForCompactionCycle(wal, parser, "ri:s");
		sleep_for(seconds(1));

		delete wal;

		Database replayDb;
		Parser replayParser(&replayDb);
		replayWal(walFile, replayParser);

		command = "TYPE ri:s ri:l ri:h";
		AssertEqual("WAL flow 20", executeCommand(replayParser, command), "String List Hash");

		std::remove(walFile.c_str());
		std::remove(compactionWalFile.c_str());
	}
}

void sanityCheck_walPersistence() {
	printHeading("Sanity Checks for WAL persistence");
	runScenarioPersistenceMixedReplay();
	runScenarioPersistenceTTLAndPersist();
	runScenarioPersistenceMultiRestart();
	runScenarioPersistenceLongMixed();
	runScenarioCompactionActiveSnapshot();
	runScenarioCompactionTempFileCleanup();
	runScenarioCompactionMultipleCycles();
	runScenarioCompactionReplayIntegrity();
}
