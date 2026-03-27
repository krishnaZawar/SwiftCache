#include<iostream>
#include<iomanip>
#include<vector>

#include "../src/parser.cpp"

#ifndef TestUtils_h
#define TestUtils_h

using std::string;
using std::cout;
using std::endl;
using std::left;
using std::setw;
using std::vector;

void printHeading(string heading) {
    int pad = 30 - heading.size()/2;
    cout << string(pad, ' ') << heading << endl;
}

void AssertEqual(string testname, string actual, string expected) {
    cout << left << setw(50) << testname << " : " << setw(10) << (actual == expected? "PASS ✅" : "FAIL ❌") << endl;
}

string executeCommand(Parser &parser, string &command) {
    string resp;
    try {
        resp = parser.parseCommand(command.c_str(), command.length());
    } catch (string &err) {
        resp = err;
    }
    return resp;
}

void executeFlow(string flowName, Parser &parser, vector<string> &commands, string expectedOutput, int outputIndex) {
    int n = commands.size();
    for(int i = 0; i < n; i++) {
        if (outputIndex != i) {
            executeCommand(parser, commands[i]);
        } else {
            AssertEqual(flowName, executeCommand(parser, commands[i]), expectedOutput);
        }
    }
}

#endif // TestUtils_h