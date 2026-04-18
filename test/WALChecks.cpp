#include<fstream>
#include "test_utils.h"

using std::ifstream;
using std::ofstream;

void sanityCheck_walPersistence() {
    printHeading("Basic Checks for WAL Loading correctness");

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

    // Scenario 1
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

    // Scenario 2
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

    // Scenario 3
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

    // Scenario 4
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