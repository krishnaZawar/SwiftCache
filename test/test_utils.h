#include<iostream>
#include<iomanip>

#include "../src/parser.cpp"

using std::string;
using std::cout;
using std::endl;
using std::left;
using std::setw;

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