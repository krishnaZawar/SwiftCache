#include "test_utils.h"
#include "../src/base/const.h"

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

void syntaxCheck_listCommands() {
    Database db;
    Parser parser (&db);
    string command;

// ------------------------------------------ LPUSH command -------------------------------------------------
    printHeading("Syntax Checks For LPUSH command");

    command = "LPUSH k1 1";
    AssertEqual("Correct syntax", executeCommand(parser, command), "1");

    command = "LPUSH k1 2 3 4";
    AssertEqual("Bulk insert", executeCommand(parser, command), "1");
    
    command = "LPUS k1 1";
    AssertEqual("Invalid command name", executeCommand(parser, command), ERR_NO_COMM);
    
    command = "LPUSH";
    AssertEqual("Only command name passed", executeCommand(parser, command), ERR_LPUSH_COMM);

    command = "LPUSH k1";
    AssertEqual("No values passed", executeCommand(parser, command), ERR_LPUSH_COMM);
    
    cout <<endl;

// ------------------------------------------ RPUSH command -------------------------------------------------
    printHeading("Syntax Checks For RPUSH command");

    command = "RPUSH k1 5";
    AssertEqual("Correct syntax", executeCommand(parser, command), "1");

    command = "RPUSH k1 6 7 8";
    AssertEqual("Bulk insert", executeCommand(parser, command), "1");
    
    command = "RPUS k1 1";
    AssertEqual("Invalid command name", executeCommand(parser, command), ERR_NO_COMM);
    
    command = "RPUSH";
    AssertEqual("Only command name passed", executeCommand(parser, command), ERR_RPUSH_COMM);

    command = "RPUSH k1";
    AssertEqual("No values passed", executeCommand(parser, command), ERR_RPUSH_COMM);
    
    cout <<endl;

// ------------------------------------------ LPUSHIDX command -------------------------------------------------
    printHeading("Syntax Checks For LPUSHIDX command");

    command = "LPUSHIDX k1 0 0";
    AssertEqual("Correct syntax", executeCommand(parser, command), "1");
    
    command = "LPUSHID k1 1";
    AssertEqual("Invalid command name", executeCommand(parser, command), ERR_NO_COMM);
    
    command = "LPUSHIDX";
    AssertEqual("Only command name passed", executeCommand(parser, command), ERR_LPUSHIDX_COMM);

    command = "LPUSHIDX k1 1";
    AssertEqual("Index not passed", executeCommand(parser, command), ERR_LPUSHIDX_COMM);

    command = "LPUSHIDX k1";
    AssertEqual("Index and value not passed", executeCommand(parser, command), ERR_LPUSHIDX_COMM);

    command = "LPUSHIDX k1 1 a1";
    AssertEqual("Index is not an integer", executeCommand(parser, command), ERR_EXPECTED_INTEGER);

    command = "LPUSHIDX k1 1  1 1";
    AssertEqual("Extra params passed", executeCommand(parser, command), ERR_LPUSHIDX_COMM);
    
    cout <<endl;

// ------------------------------------------ LPOP command -------------------------------------------------
    printHeading("Syntax Checks For LPOP command");

    command = "LPOP k1 2";
    AssertEqual("Correct syntax", executeCommand(parser, command), "1");
    
    command = "LPO k1 1";
    AssertEqual("Invalid command name", executeCommand(parser, command), ERR_NO_COMM);
    
    command = "LPOP";
    AssertEqual("Only command name passed", executeCommand(parser, command), ERR_LPOP_COMM);

    command = "LPOP k1";
    AssertEqual("Count not passed", executeCommand(parser, command), ERR_LPOP_COMM);

    command = "LPOP k1 a1";
    AssertEqual("Count is not an integer", executeCommand(parser, command), ERR_EXPECTED_INTEGER);

    command = "LPOP k1 1 1";
    AssertEqual("Extra params passed", executeCommand(parser, command), ERR_LPOP_COMM);
    
    cout <<endl;

// ------------------------------------------ RPOP command -------------------------------------------------
    printHeading("Syntax Checks For RPOP command");

    command = "RPOP k1 1";
    AssertEqual("Correct syntax", executeCommand(parser, command), "1");
    
    command = "RPO k1 1";
    AssertEqual("Invalid command name", executeCommand(parser, command), ERR_NO_COMM);
    
    command = "RPOP";
    AssertEqual("Only command name passed", executeCommand(parser, command), ERR_RPOP_COMM);

    command = "RPOP k1";
    AssertEqual("Count not passed", executeCommand(parser, command), ERR_RPOP_COMM);

    command = "RPOP k1 a1";
    AssertEqual("Count is not an integer", executeCommand(parser, command), ERR_EXPECTED_INTEGER);

    command = "RPOP k1 1 1";
    AssertEqual("Extra params passed", executeCommand(parser, command), ERR_RPOP_COMM);
    
    cout <<endl;

// ------------------------------------------ LPOPIDX command -------------------------------------------------
    printHeading("Syntax Checks For LPOPIDX command");

    command = "LPOPIDX k1 0";
    AssertEqual("Correct syntax", executeCommand(parser, command), "1");
    
    command = "LPOPID k1 1";
    AssertEqual("Invalid command name", executeCommand(parser, command), ERR_NO_COMM);
    
    command = "LPOPIDX";
    AssertEqual("Only command name passed", executeCommand(parser, command), ERR_LPOPIDX_COMM);

    command = "LPOPIDX k1";
    AssertEqual("Count not passed", executeCommand(parser, command), ERR_LPOPIDX_COMM);

    command = "LPOPIDX k1 a1";
    AssertEqual("Count is not an integer", executeCommand(parser, command), ERR_EXPECTED_INTEGER);

    command = "LPOPIDX k1 1 1";
    AssertEqual("Extra params passed", executeCommand(parser, command), ERR_LPOPIDX_COMM);
    
    cout <<endl;

// ------------------------------------------ LLEN command -------------------------------------------------
    printHeading("Syntax Checks For LLEN command");

    command = "LLEN k1";
    AssertEqual("Correct syntax", executeCommand(parser, command), "5");
    
    command = "LLE key";
    AssertEqual("Invalid command name", executeCommand(parser, command), ERR_NO_COMM);
    
    command = "LLEN";
    AssertEqual("Only command name passed", executeCommand(parser, command), ERR_LLEN_COMM);

    command = "LLEN k1 1 1";
    AssertEqual("Extra params passed", executeCommand(parser, command), ERR_LLEN_COMM);
    
    cout <<endl;

// ------------------------------------------ LINDEX command -------------------------------------------------
    printHeading("Syntax Checks For LINDEX command");

    command = "LINDEX k1 0";
    AssertEqual("Correct syntax", executeCommand(parser, command), "2");

    command = "LINDEX k1 0 1 2 3";
    AssertEqual("Bulk fetch", executeCommand(parser, command), "2\n1\n5\n6");
    
    command = "LINDE k1 1";
    AssertEqual("Invalid command name", executeCommand(parser, command), ERR_NO_COMM);
    
    command = "LINDEX";
    AssertEqual("Only command name passed", executeCommand(parser, command), ERR_LINDEX_COMM);

    command = "LINDEX k1";
    AssertEqual("Index not passed", executeCommand(parser, command), ERR_LINDEX_COMM);

    command = "LINDEX k1 a1";
    AssertEqual("Index passed is not an integer", executeCommand(parser, command), ERR_EXPECTED_INTEGER);
    
    cout <<endl;

// ------------------------------------------ LRANGE command -------------------------------------------------
    printHeading("Syntax Checks For LRANGE command");

    command = "LRANGE k1 0 3";
    AssertEqual("Correct syntax", executeCommand(parser, command), "2\n1\n5\n6");
    
    command = "LRANG k1 1";
    AssertEqual("Invalid command name", executeCommand(parser, command), ERR_NO_COMM);
    
    command = "LRANGE";
    AssertEqual("Only command name passed", executeCommand(parser, command), ERR_LRANGE_COMM);

    command = "LRANGE k1 0";
    AssertEqual("End index is not passed", executeCommand(parser, command), ERR_LRANGE_COMM);

    command = "LRANGE k1";
    AssertEqual("Start index and end index is not passed", executeCommand(parser, command), ERR_LRANGE_COMM);

    command = "LRANGE k1 0 a1";
    AssertEqual("End index is not an integer", executeCommand(parser, command), ERR_EXPECTED_INTEGER);

    command = "LRANGE k1 0a 1";
    AssertEqual("Start index is not an integer", executeCommand(parser, command), ERR_EXPECTED_INTEGER);

    command = "LRANGE k1 1 1 1";
    AssertEqual("Extra params passed", executeCommand(parser, command), ERR_LRANGE_COMM);
    
    cout <<endl;
}

void syntaxCheck_hashCommands() {
    Database db;
    Parser parser (&db);
    string command;

// ------------------------------------------ HSET command -------------------------------------------------
    printHeading("Syntax Checks For HSET command");

    command = "HSET k1 f6 v6";
    AssertEqual("Correct syntax", executeCommand(parser, command), "1");

    command = "HSET k1 f5 v5 f4 v4 f3 v3 f2 v2 f1 v1";
    AssertEqual("Bulk write", executeCommand(parser, command), "1");
    
    command = "HSE k1 1";
    AssertEqual("Invalid command name", executeCommand(parser, command), ERR_NO_COMM);
    
    command = "HSET";
    AssertEqual("Only command name passed", executeCommand(parser, command), ERR_HSET_COMM);

    command = "HSET k1";
    AssertEqual("No field-value pairs passed", executeCommand(parser, command), ERR_HSET_COMM);

    command = "HSET k1 f1";
    AssertEqual("Field-value pairs incomplete", executeCommand(parser, command), ERR_HSET_COMM);
    
    cout <<endl;

// ------------------------------------------ HGET command -------------------------------------------------
    printHeading("Syntax Checks For HGET command");

    command = "HGET k1 f1";
    AssertEqual("Correct syntax", executeCommand(parser, command), "v1");

    command = "HGET k1 f1 f2 f3";
    AssertEqual("Bulk fetch", executeCommand(parser, command), "v1\nv2\nv3");
    
    command = "HGE k1 1";
    AssertEqual("Invalid command name", executeCommand(parser, command), ERR_NO_COMM);
    
    command = "HGET";
    AssertEqual("Only command name passed", executeCommand(parser, command), ERR_HGET_COMM);

    command = "HGET k1";
    AssertEqual("No field name passed", executeCommand(parser, command), ERR_HGET_COMM);
    
    cout <<endl;

// ------------------------------------------ HGETALL command -------------------------------------------------
    printHeading("Syntax Checks For HGETALL command");

    command = "HGETALL k1";
    AssertEqual("Correct syntax", executeCommand(parser, command), "f1 : v1\nf2 : v2\nf4 : v4\nf5 : v5\nf3 : v3\nf6 : v6");
    
    command = "HGETAL k1";
    AssertEqual("Invalid command name", executeCommand(parser, command), ERR_NO_COMM);
    
    command = "HGETALL";
    AssertEqual("Only command name passed", executeCommand(parser, command), ERR_HGETALL_COMM);

    command = "HGETALL k1 1 1";
    AssertEqual("Extra params passed", executeCommand(parser, command), ERR_HGETALL_COMM);
    
    cout <<endl;

// ------------------------------------------ HKEYS command -------------------------------------------------
    printHeading("Syntax Checks For HKEYS command");

    command = "HKEYS k1";
    AssertEqual("Correct syntax", executeCommand(parser, command), "f1 f2 f4 f5 f3 f6");
    
    command = "HKEY k1";
    AssertEqual("Invalid command name", executeCommand(parser, command), ERR_NO_COMM);
    
    command = "HKEYS";
    AssertEqual("Only command name passed", executeCommand(parser, command), ERR_HKEYS_COMM);

    command = "HKEYS k1 1 1";
    AssertEqual("Extra params passed", executeCommand(parser, command), ERR_HKEYS_COMM);
    
    cout <<endl;

// ------------------------------------------ HDEL command -------------------------------------------------
    printHeading("Syntax Checks For HDEL command");

    command = "HDEL k1 f6";
    AssertEqual("Correct syntax", executeCommand(parser, command), "1");

    command = "HDEL k1 f6 f5 f4";
    AssertEqual("Bulk delete", executeCommand(parser, command), "011");
    
    command = "HDE k1";
    AssertEqual("Invalid command name", executeCommand(parser, command), ERR_NO_COMM);
    
    command = "HDEL";
    AssertEqual("Only command name passed", executeCommand(parser, command), ERR_HDEL_COMM);

    command = "HDEL k1";
    AssertEqual("No field name passed", executeCommand(parser, command), ERR_HDEL_COMM);
    
    cout <<endl;

// ------------------------------------------ HEXISTS command -------------------------------------------------
    printHeading("Syntax Checks For HEXISTS command");

    command = "HEXISTS k1 f1";
    AssertEqual("Correct syntax", executeCommand(parser, command), "1");

    command = "HEXISTS k1 f1 f2 f3 f4";
    AssertEqual("Bulk check", executeCommand(parser, command), "1110");
    
    command = "HEXIST k1";
    AssertEqual("Invalid command name", executeCommand(parser, command), ERR_NO_COMM);
    
    command = "HEXISTS";
    AssertEqual("Only command name passed", executeCommand(parser, command), ERR_HEXISTS_COMM);

    command = "HEXISTS k1";
    AssertEqual("No field name passed", executeCommand(parser, command), ERR_HEXISTS_COMM);
    
    cout <<endl;

// ------------------------------------------ HLEN command -------------------------------------------------
    printHeading("Syntax Checks For HLEN command");

    command = "HLEN k1";
    AssertEqual("Correct syntax", executeCommand(parser, command), "3");
    
    command = "HLE k1";
    AssertEqual("Invalid command name", executeCommand(parser, command), ERR_NO_COMM);
    
    command = "HLEN";
    AssertEqual("Only command name passed", executeCommand(parser, command), ERR_HLEN_COMM);

    command = "HLEN k1 1 1";
    AssertEqual("Extra params passed", executeCommand(parser, command), ERR_HLEN_COMM);
    
    cout <<endl;

// ------------------------------------------ HSTRLEN command -------------------------------------------------
    printHeading("Syntax Checks For HSTRLEN command");

    command = "HSTRLEN k1 f1";
    AssertEqual("Correct syntax", executeCommand(parser, command), "2");

    command = "HSTRLEN k1 f1 f2 f3";
    AssertEqual("Correct syntax", executeCommand(parser, command), "2\n2\n2");
    
    command = "HSTRLE k1";
    AssertEqual("Invalid command name", executeCommand(parser, command), ERR_NO_COMM);
    
    command = "HSTRLEN";
    AssertEqual("Only command name passed", executeCommand(parser, command), ERR_HSTRLEN_COMM);

    command = "HSTRLEN k1";
    AssertEqual("No field name passed", executeCommand(parser, command), ERR_HSTRLEN_COMM);
    
    cout <<endl;
}