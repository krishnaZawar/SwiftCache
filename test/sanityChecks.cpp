#include<thread>
#include<chrono>
#include<fstream>
#include<cstdio>
#include<deque>
#include<random>

#include "test_utils.h"
#include "../src/base/errors.h"

using std::this_thread::sleep_for;
using std::chrono::seconds;
using std::ofstream;
using std::ifstream;
using std::getline;
using std::deque;
using std::mt19937;
using std::uniform_int_distribution;

void sanityCheck_stringCommands() {
    Database db;
    Parser parser (&db);
    vector<string> commands;

    printHeading("Sanity Checks for String flows");

    // flow 1
    commands = {
        "SET k v",
        "GET k"
    };
    executeFlow("flow 1", parser, commands, "v", 1);

    // flow 2
    commands = {
        "SET k1 v1",
        "GET k2"
    };
    executeFlow("flow 2", parser, commands, ERR_KEY_NOT_FOUND+"\nError fetching k2", 1);

    // flow 3
    commands = {
        "SET k1 v1 k2 v2 k3 v3",
        "GET k1 k2 k3"
    };
    executeFlow("flow 3", parser, commands, "v1\nv2\nv3", 1);
}

void sanityCheck_genericCommands() {
    Database db;
    Parser parser (&db);
    string command;
    vector<string> commands;

    printHeading("Sanity Checks for Generic Command flows");

    // flow 1
    commands = {
        "SET k v",
        "DEL k",
        "GET k"
    };
    executeFlow("flow 1", parser, commands, ERR_KEY_NOT_FOUND+"\nError fetching k", 2);

    // flow 2
    command = "DEL k";
    AssertEqual("flow 2", executeCommand(parser, command), "0");

    // flow 3
    commands = {
        "SET k v",
        "TYPE k",
        "DEL k",    // cleanup command
    };
    executeFlow("flow 3", parser, commands, "String", 1);
    
    // flow 4
    command = "TYPE k";
    AssertEqual("flow 4", executeCommand(parser, command), "nil");
    
    // flow 5
    commands = {
        "SET k1 v",
        "LPUSH k2 v",
        "HSET k3 f v",
        "TYPE k1 k2 k3 k4",
        "DEL k1 k2 k3"  // cleanup command
    };
    executeFlow("flow 5", parser, commands, "String List Hash nil", 3);

    // flow 6
    command = "SET k v";
    executeCommand(parser, command);
    command = "EXPIRE k 1";
    executeCommand(parser, command);
    sleep_for(seconds(1));  // sleep to simulate expiry
    command = "GET k";
    AssertEqual("flow 6", executeCommand(parser, command), ERR_KEY_NOT_FOUND+"\nError fetching k");

    // flow 7
    command = "EXPIRE k 1";
    AssertEqual("flow 7", executeCommand(parser, command), "0");

    // flow 8
    command = "SET k v";
    executeCommand(parser, command);
    command = "EXPIRE k 1";
    executeCommand(parser, command);
    command = "PERSIST k";
    executeCommand(parser, command);
    sleep_for(seconds(1));  // sleep for true persistence check
    command = "GET k";
    AssertEqual("flow 8", executeCommand(parser, command), "v");
    command = "DEL k";  // cleanup command
    executeCommand(parser, command);

    // flow 9
    command = "PERSIST k";
    AssertEqual("flow 9", executeCommand(parser, command), "0");

    // flow 10
    commands = {
        "SET k v",
        "EXPIRE k -1"
    };
    executeFlow("flow 10", parser, commands, "0", 1);
}

void sanityCheck_listCommands() {
    Database db;
    Parser parser (&db);
    string command;
    vector<string> commands;

    printHeading("Sanity Checks for List Command flows");

    // flow 1
    commands = {
        "LPUSH k 1 2 3",
        "LPOP k 3",
        "DEL k" // cleanup command
    };
    executeFlow("flow 1", parser, commands, "1", 1);

    // flow 2
    commands = {
        "LPUSH k 1 2 3",
        "LPOP k -1",
        "DEL k" // cleanup command
    };
    executeFlow("flow 2", parser, commands, ERR_POP_COUNT_UNDERFLOW, 1);

    // flow 3
    commands = {
        "LPUSH k 1 2 3",
        "LPOP k 4",
        "DEL k" // cleanup command
    };
    executeFlow("flow 3", parser, commands, ERR_POP_COUNT_OVERFLOW, 1);

    // flow 4
    commands = {
        "RPUSH k 1 2 3",
        "RPOP k 3",
        "DEL k" // cleanup command
    }; 
    executeFlow("flow 4", parser, commands, "1", 1);

    // flow 5
    commands = {
        "RPUSH k 1 2 3",
        "RPOP k -1",
        "DEL k" // cleanup command
    };
    executeFlow("flow 5", parser, commands, ERR_POP_COUNT_UNDERFLOW, 1);

    // flow 6
    commands = {
        "RPUSH k 1 2 3",
        "RPOP k 4",
        "DEL k" // cleanup command
    };
    executeFlow("flow 6", parser, commands, ERR_POP_COUNT_OVERFLOW, 1);

    // flow 7
    commands = {
        "LPUSH k 1 2 3",
        "LPUSHIDX k 0 0",
        "DEL k" // cleanup command
    };
    executeFlow("flow 7", parser, commands, "1", 1);

    // flow 8
    commands = {
        "LPUSH k 1 2 3",
        "LPUSHIDX k 0 -1",
        "DEL k" // cleanup command
    };
    executeFlow("flow 8", parser, commands, ERR_INDEX_OUT_OF_BOUNDS, 1);

    // flow 9
    commands = {
        "LPUSH k 1 2 3",
        "LPUSHIDX k 0 3",
        "DEL k" // cleanup command
    };

    // flow 10
    commands = {
        "LPUSH k 1 2 3",
        "LPOPIDX k 0",
        "DEL k" // cleanup command
    };
    executeFlow("flow 10", parser, commands, "1", 1);

    // flow 11
    commands = {
        "LPUSH k 1 2 3",
        "LPOPIDX k -1",
        "DEL k" // cleanup command
    };
    executeFlow("flow 11", parser, commands, ERR_INDEX_OUT_OF_BOUNDS, 1);

    // flow 12
    commands = {
        "LPUSH k 1 2 3",
        "LPOPIDX k 3",
        "DEL k" // cleanup command
    };
    executeFlow("flow 12", parser, commands, ERR_INDEX_OUT_OF_BOUNDS, 1);

    // flow 13
    commands = {
        "LPUSH k 1 2 3",
        "LLEN k",
        "DEL k" // cleanup command
    };
    executeFlow("flow 13", parser, commands, "3", 1);

    // flow 14
    commands = {
        "LPUSH k 1 2 3",
        "LINDEX k 0",
        "DEL k" // cleanup command
    };
    executeFlow("flow 14", parser, commands, "3", 1);

    // flow 15
    commands = {
        "LPUSH k 1 2 3",
        "LINDEX k -1",
        "DEL k" // cleanup command
    };
    executeFlow("flow 15", parser, commands, ERR_INDEX_OUT_OF_BOUNDS, 1);

    // flow 16
    commands = {
        "LPUSH k 1 2 3",
        "LINDEX k 3",
        "DEL k" // cleanup command
    };
    executeFlow("flow 16", parser, commands, ERR_INDEX_OUT_OF_BOUNDS, 1);

    // flow 17
    commands = {
        "LPUSH k 1 2 3",
        "LRANGE k 0 2",
        "DEL k" // cleanup command
    };
    executeFlow("flow 17", parser, commands, "3\n2\n1", 1);

    // flow 18
    commands = {
        "LPUSH k 1 2 3",
        "LRANGE k -1 2",
        "DEL k" // cleanup command
    };
    executeFlow("flow 18", parser, commands, ERR_INDEX_OUT_OF_BOUNDS, 1);

    // flow 19
    commands = {
        "LPUSH k 1 2 3",
        "LRANGE k 0 3",
        "DEL k" // cleanup command
    };
    executeFlow("flow 19", parser, commands, ERR_INDEX_OUT_OF_BOUNDS, 1);

    // flow 20
    command = "LLEN k";
    AssertEqual("flow 20", executeCommand(parser, command), ERR_KEY_NOT_FOUND);

    // flow 21
    command = "LPOP k 1";
    AssertEqual("flow 21", executeCommand(parser, command), ERR_KEY_NOT_FOUND);

    // flow 22
    command = "RPOP k 1";
    AssertEqual("flow 22", executeCommand(parser, command), ERR_KEY_NOT_FOUND);

    // flow 23
    command = "LPOPIDX k 1";
    AssertEqual("flow 23", executeCommand(parser, command), ERR_KEY_NOT_FOUND);

    // flow 24
    command = "LPUSHIDX k 1 0";
    AssertEqual("flow 24", executeCommand(parser, command), ERR_KEY_NOT_FOUND);

    // flow 25
    command = "LINDEX k 1";
    AssertEqual("flow 25", executeCommand(parser, command), ERR_KEY_NOT_FOUND);

    // flow 26
    command = "LRANGE k 1 2";
    AssertEqual("flow 26", executeCommand(parser, command), ERR_KEY_NOT_FOUND);
}

void sanityCheck_hashCommands() {
    Database db;
    Parser parser (&db);
    string command;
    vector<string> commands;

    printHeading("Sanity Checks for Hash Command flows");

    // flow 1
    commands = {
        "HSET k f1 v1 f2 v2",
        "HGET k f1 f2",
        "DEL k" // cleanup command
    };
    executeFlow("flow 1", parser, commands, "v1\nv2", 1);

    // flow 2
    commands = {
        "HSET k f1 v1 f2 v2",
        "HGETALL k",
        "DEL k" // cleanup command
    };
    executeFlow("flow 2", parser, commands, "f2 : v2\nf1 : v1", 1);

    // flow 3
    commands = {
        "HSET k f1 v1 f2 v2",
        "HKEYS k",
        "DEL k" // cleanup command
    };
    executeFlow("flow 3", parser, commands, "f2 f1", 1);

    // flow 4
    commands = {
        "HSET k f1 v1",
        "HDEL k f1 f2",
        "DEL k" // cleanup command
    };
    executeFlow("flow 4", parser, commands, "10", 1);

    // flow 5
    commands = {
        "HSET k f1 v1",
        "HEXISTS k f1 f2",
        "DEL k" // cleanup command
    };
    executeFlow("flow 5", parser, commands, "10", 1);

    // flow 6
    commands = {
        "HSET k f1 v1",
        "HLEN k",
        "DEL k" // cleanup command
    };
    executeFlow("flow 6", parser, commands, "1", 1);

    // flow 7
    commands = {
        "HSET k f1 v1 f2 v2",
        "HSTRLEN k f1 f2",
        "DEL k" // cleanup command
    };
    executeFlow("flow 7", parser, commands, "2\n2", 1);

    // flow 8
    commands = {
        "HSET k f1 v1 f2 v2",
        "HSTRLEN k f3",
        "DEL k" // cleanup command
    };
    executeFlow("flow 8", parser, commands, ERR_HASH_FIELD_DOES_NOT_EXIST + "\nError fetching field f3", 1);

    // flow 9
    command = "HGET k f1";
    AssertEqual("flow 9", executeCommand(parser, command), ERR_KEY_NOT_FOUND);

    // flow 10
    command = "HGETALL k";
    AssertEqual("flow 10", executeCommand(parser, command), ERR_KEY_NOT_FOUND);

    // flow 11
    command = "HKEYS k";
    AssertEqual("flow 11", executeCommand(parser, command), ERR_KEY_NOT_FOUND);

    // flow 12
    command = "HDEL k f1";
    AssertEqual("flow 12", executeCommand(parser, command), ERR_KEY_NOT_FOUND);

    // flow 13
    command = "HEXISTS k f1";
    AssertEqual("flow 13", executeCommand(parser, command), ERR_KEY_NOT_FOUND);

    // flow 14
    command = "HLEN k";
    AssertEqual("flow 14", executeCommand(parser, command), ERR_KEY_NOT_FOUND);

    // flow 15
    command = "HSTRLEN k f1";
    AssertEqual("flow 15", executeCommand(parser, command), ERR_KEY_NOT_FOUND);
}

void sanityCheck_genericFlows() {
    Database db;
    Parser parser (&db);
    string command;

    printHeading("Sanity Checks for Generic flows");

    // flow 1 - 17
    command = "SET k v";
    executeCommand(parser, command);
    command = "HSET k f1 v1";
    AssertEqual("flow 1", executeCommand(parser, command), ERR_TYPE_MISMATCH);
    command = "HGET k f1";
    AssertEqual("flow 2", executeCommand(parser, command), ERR_TYPE_MISMATCH);
    command = "HGETALL k";
    AssertEqual("flow 3", executeCommand(parser, command), ERR_TYPE_MISMATCH);
    command = "HKEYS k";
    AssertEqual("flow 4", executeCommand(parser, command), ERR_TYPE_MISMATCH);
    command = "HDEL k f1";
    AssertEqual("flow 5", executeCommand(parser, command), ERR_TYPE_MISMATCH);
    command = "HEXISTS k f1";
    AssertEqual("flow 6", executeCommand(parser, command), ERR_TYPE_MISMATCH);
    command = "HLEN k";
    AssertEqual("flow 7", executeCommand(parser, command), ERR_TYPE_MISMATCH);
    command = "HSTRLEN k f1";
    AssertEqual("flow 8", executeCommand(parser, command), ERR_TYPE_MISMATCH);
    command = "LPUSH k v";
    AssertEqual("flow 9", executeCommand(parser, command), ERR_TYPE_MISMATCH);
    command = "RPUSH k v";
    AssertEqual("flow 10", executeCommand(parser, command), ERR_TYPE_MISMATCH);
    command = "LPUSHIDX k v 1";
    AssertEqual("flow 11", executeCommand(parser, command), ERR_TYPE_MISMATCH);
    command = "LPOP k 1";
    AssertEqual("flow 12", executeCommand(parser, command), ERR_TYPE_MISMATCH);
    command = "RPOP k 1";
    AssertEqual("flow 13", executeCommand(parser, command), ERR_TYPE_MISMATCH);
    command = "LPOPIDX k 1";
    AssertEqual("flow 14", executeCommand(parser, command), ERR_TYPE_MISMATCH);
    command = "LLEN k";
    AssertEqual("flow 15", executeCommand(parser, command), ERR_TYPE_MISMATCH);
    command = "LINDEX k 1";
    AssertEqual("flow 16", executeCommand(parser, command), ERR_TYPE_MISMATCH);
    command = "LRANGE k 1 2";
    AssertEqual("flow 17", executeCommand(parser, command), ERR_TYPE_MISMATCH);
    command = "DEL k";  // cleanup command
    executeCommand(parser, command);
    
    // flow 18 - 27
    command = "LPUSH k v";
    executeCommand(parser, command);
    command = "HSET k f1 v1";
    AssertEqual("flow 18", executeCommand(parser, command), ERR_TYPE_MISMATCH);
    command = "HGET k f1";
    AssertEqual("flow 19", executeCommand(parser, command), ERR_TYPE_MISMATCH);
    command = "HGETALL k";
    AssertEqual("flow 20", executeCommand(parser, command), ERR_TYPE_MISMATCH);
    command = "HKEYS k";
    AssertEqual("flow 21", executeCommand(parser, command), ERR_TYPE_MISMATCH);
    command = "HDEL k f1";
    AssertEqual("flow 22", executeCommand(parser, command), ERR_TYPE_MISMATCH);
    command = "HEXISTS k f1";
    AssertEqual("flow 23", executeCommand(parser, command), ERR_TYPE_MISMATCH);
    command = "HLEN k";
    AssertEqual("flow 24", executeCommand(parser, command), ERR_TYPE_MISMATCH);
    command = "HSTRLEN k f1";
    AssertEqual("flow 25", executeCommand(parser, command), ERR_TYPE_MISMATCH);
    command = "SET k v";
    AssertEqual("flow 26", executeCommand(parser, command), ERR_TYPE_MISMATCH + "\nError inserting k");
    command = "GET k";
    AssertEqual("flow 27", executeCommand(parser, command), ERR_TYPE_MISMATCH + "\nError fetching k");
    command = "DEL k";  // cleanup command
    executeCommand(parser, command);

    // flow 28 - 38
    command = "HSET k f v";
    executeCommand(parser, command);
    command = "LPUSH k v";
    AssertEqual("flow 28", executeCommand(parser, command), ERR_TYPE_MISMATCH);
    command = "RPUSH k v";
    AssertEqual("flow 29", executeCommand(parser, command), ERR_TYPE_MISMATCH);
    command = "LPUSHIDX k v 1";
    AssertEqual("flow 30", executeCommand(parser, command), ERR_TYPE_MISMATCH);
    command = "LPOP k 1";
    AssertEqual("flow 31", executeCommand(parser, command), ERR_TYPE_MISMATCH);
    command = "RPOP k 1";
    AssertEqual("flow 32", executeCommand(parser, command), ERR_TYPE_MISMATCH);
    command = "LPOPIDX k 1";
    AssertEqual("flow 33", executeCommand(parser, command), ERR_TYPE_MISMATCH);
    command = "LLEN k";
    AssertEqual("flow 34", executeCommand(parser, command), ERR_TYPE_MISMATCH);
    command = "LINDEX k 1";
    AssertEqual("flow 35", executeCommand(parser, command), ERR_TYPE_MISMATCH);
    command = "LRANGE k 1 2";
    AssertEqual("flow 36", executeCommand(parser, command), ERR_TYPE_MISMATCH);
    command = "SET k v";
    AssertEqual("flow 37", executeCommand(parser, command), ERR_TYPE_MISMATCH + "\nError inserting k");
    command = "GET k";
    AssertEqual("flow 38", executeCommand(parser, command), ERR_TYPE_MISMATCH + "\nError fetching k");
}

void sanityCheck_walPersistence() {
    printHeading("Sanity Checks for WAL persistence");

    auto replayWal = [&](const string &walFile, Parser &parser) {
        ifstream file(walFile);
        if(file) {
            string walLog;
            while(getline(file, walLog)) {
                try {
                    parser.parseCommand(walLog.c_str(), walLog.size(), true);
                } catch(...) {}
            }
        }
    };

    // Scenario 1: mixed replay with invalid command and overwrite semantics.
    const string walFile1 = "test/wal_persistence_test_dump_1.txt";
    {
        ofstream file(walFile1, std::ios::trunc);
        file << "SET persistedKey oldValue\n";
        file << "SET persistedKey persistedValue\n";
        file << "DEL persistedList\n";
        file << "RPUSH persistedList 1 2 3\n";
        file << "LPUSH persistedList 0\n";
        file << "HSET persistedHash field1 value1 field2 value2\n";
        file << "INVALIDCMD should be ignored\n";
        file << "SET postErrorKey stillLoaded\n";
    }

    Database db1;
    Parser parser1 (&db1);
    replayWal(walFile1, parser1);

    string command;
    command = "GET persistedKey";
    AssertEqual("WAL flow 1", executeCommand(parser1, command), "persistedValue");

    command = "LLEN persistedList";
    AssertEqual("WAL flow 2", executeCommand(parser1, command), "4");

    command = "LRANGE persistedList 0 3";
    AssertEqual("WAL flow 3", executeCommand(parser1, command), "0\n1\n2\n3");

    command = "HGET persistedHash field1 field2";
    AssertEqual("WAL flow 4", executeCommand(parser1, command), "value1\nvalue2");

    command = "GET postErrorKey";
    AssertEqual("WAL flow 5", executeCommand(parser1, command), "stillLoaded");

    std::remove(walFile1.c_str());

    // Scenario 2: repeated list snapshots remain stable with DEL + RPUSH contract.
    const string walFile2 = "test/wal_persistence_test_dump_2.txt";
    {
        ofstream file(walFile2, std::ios::trunc);
        file << "DEL compactedList\n";
        file << "RPUSH compactedList a b c\n";
        file << "DEL compactedList\n";
        file << "RPUSH compactedList a b c\n";
        file << "LPUSH compactedList z\n";
    }

    Database db2;
    Parser parser2 (&db2);
    replayWal(walFile2, parser2);

    command = "LLEN compactedList";
    AssertEqual("WAL flow 6", executeCommand(parser2, command), "4");

    command = "LRANGE compactedList 0 3";
    AssertEqual("WAL flow 7", executeCommand(parser2, command), "z\na\nb\nc");

    std::remove(walFile2.c_str());

    // Scenario 3: malformed command does not block subsequent valid replay with quoted values.
    const string walFile3 = "test/wal_persistence_test_dump_3.txt";
    {
        ofstream file(walFile3, std::ios::trunc);
        file << "SET stableKey stableValue\n";
        file << "SET broken \"missing quote\n"; // intentionally malformed
        file << "SET afterMalformed stillWorks\n";
        file << "DEL quotedList\n";
        file << "RPUSH quotedList \"hello world\" \"swift cache\"\n";
    }

    Database db3;
    Parser parser3 (&db3);
    replayWal(walFile3, parser3);

    command = "GET stableKey";
    AssertEqual("WAL flow 8", executeCommand(parser3, command), "stableValue");

    command = "GET afterMalformed";
    AssertEqual("WAL flow 9", executeCommand(parser3, command), "stillWorks");

    command = "LRANGE quotedList 0 1";
    AssertEqual("WAL flow 10", executeCommand(parser3, command), "hello world\nswift cache");

    std::remove(walFile3.c_str());

    // Scenario 4: repeated DEL + rebuild remains deterministic across list and hash keys.
    const string walFile4 = "test/wal_persistence_test_dump_4.txt";
    {
        ofstream file(walFile4, std::ios::trunc);
        file << "DEL rebuildList\n";
        file << "DEL rebuildList\n";
        file << "RPUSH rebuildList 10 20\n";
        file << "DEL rebuildList\n";
        file << "RPUSH rebuildList 30 40 50\n";
        file << "HSET rebuildHash f1 v1\n";
        file << "DEL rebuildHash\n";
        file << "HSET rebuildHash f2 v2\n";
    }

    Database db4;
    Parser parser4 (&db4);
    replayWal(walFile4, parser4);

    command = "LLEN rebuildList";
    AssertEqual("WAL flow 11", executeCommand(parser4, command), "3");

    command = "LRANGE rebuildList 0 2";
    AssertEqual("WAL flow 12", executeCommand(parser4, command), "30\n40\n50");

    command = "HEXISTS rebuildHash f1 f2";
    AssertEqual("WAL flow 13", executeCommand(parser4, command), "01");

    std::remove(walFile4.c_str());
}