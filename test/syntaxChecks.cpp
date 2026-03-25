#include "test_utils.h"
#include "../src/constants/const.h"

void syntaxCheck_stringCommands() {
    Database db;
    Parser parser (&db);
    string command;

// ------------------------------------------ SET command -------------------------------------------------
    printHeading("Syntax Checks For SET command");

    command = "SET key val";
    AssertEqual("Correct syntax", executeCommand(parser, command), "1");

    command = "SET k1 v1 k2 v2";
    AssertEqual("Bulk write", executeCommand(parser, command), "1");
    
    command = "SE key val";
    AssertEqual("Invalid command name", executeCommand(parser, command), ERR_NO_COMM);
    
    command = "SET";
    AssertEqual("Only command name passed", executeCommand(parser, command), ERR_SET_COMM);
    
    command = "SET key";
    AssertEqual("Key value pair incomplete", executeCommand(parser, command), ERR_SET_COMM);
    
    cout <<endl;

// ------------------------------------------ GET command -------------------------------------------------
    printHeading("Syntax Checks For GET command");

    command = "GET key";
    AssertEqual("Correct syntax", executeCommand(parser, command), "val");

    command = "GET k1 k2";
    AssertEqual("Bulk fetch", executeCommand(parser, command), "v1\nv2");
    
    command = "GE key";
    AssertEqual("Invalid command name", executeCommand(parser, command), ERR_NO_COMM);
    
    command = "GET";
    AssertEqual("Only command name passed", executeCommand(parser, command), ERR_GET_COMM);
    
    cout <<endl;
}

void syntaxCheck_genericCommands() {
    Database db;
    Parser parser (&db);
    string command;

// -------------------------------------------- Pre seed --------------------------------------------
    command = "SET k1 v1 k2 v2 k3 v3";
    executeCommand(parser, command);

// ------------------------------------------ TYPE command -------------------------------------------------
    printHeading("Syntax Checks For TYPE command");

    command = "TYPE k1";
    AssertEqual("Correct syntax", executeCommand(parser, command), "String");

    command = "TYPE k1 k2 k3 k4";
    AssertEqual("Bulk fetch", executeCommand(parser, command), "String String String nil");
    
    command = "TYE key";
    AssertEqual("Invalid command name", executeCommand(parser, command), ERR_NO_COMM);
    
    command = "TYPE";
    AssertEqual("Only command name passed", executeCommand(parser, command), ERR_TYPE_COMM);
    
    cout <<endl;

// ------------------------------------------ EXPIRE command -------------------------------------------------
    printHeading("Syntax Checks For EXPIRE command");

    command = "EXPIRE k1 10";
    AssertEqual("Correct syntax", executeCommand(parser, command), "1");

    command = "EXPIRE k2 10 k3 -10";
    AssertEqual("Bulk expire", executeCommand(parser, command), "10");
    
    command = "EXPR key";
    AssertEqual("Invalid command name", executeCommand(parser, command), ERR_NO_COMM);
    
    command = "EXPIRE";
    AssertEqual("Only command name passed", executeCommand(parser, command), ERR_EXPIRE_COMM);

    command = "EXPIRE key";
    AssertEqual("Key-time pair incomplete", executeCommand(parser, command), ERR_EXPIRE_COMM);
    
    command = "EXPIRE key a21";
    AssertEqual("Time is not an integer", executeCommand(parser, command), "0");
    
    cout <<endl;

// ------------------------------------------ PERSIST command -------------------------------------------------
    printHeading("Syntax Checks For PERSIST command");

    command = "PERSIST k1";
    AssertEqual("Correct syntax", executeCommand(parser, command), "1");

    command = "PERSIST k2 k3 k4";
    AssertEqual("Bulk persist", executeCommand(parser, command), "110");
    
    command = "PRSIST key";
    AssertEqual("Invalid command name", executeCommand(parser, command), ERR_NO_COMM);
    
    command = "PERSIST";
    AssertEqual("Only command name passed", executeCommand(parser, command), ERR_PERSIST_COMM);
    
    cout <<endl;

// ------------------------------------------ DEL command -------------------------------------------------
    printHeading("Syntax Checks For DEL command");

    command = "DEL k1";
    AssertEqual("Correct syntax", executeCommand(parser, command), "1");

    command = "DEL k2 k3 k4";
    AssertEqual("Bulk delete", executeCommand(parser, command), "110");
    
    command = "DELL key";
    AssertEqual("Invalid command name", executeCommand(parser, command), ERR_NO_COMM);
    
    command = "DEL";
    AssertEqual("Only command name passed", executeCommand(parser, command), ERR_DEL_COMM);
    
    cout <<endl;
}