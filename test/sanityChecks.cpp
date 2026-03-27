#include<thread>
#include<chrono>

#include "test_utils.h"
#include "../src/base/const.h"

using std::this_thread::sleep_for;
using std::chrono::seconds;

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
