#include<string>

#include "../base/errors.h"

#ifndef strUtils
#define strUtils

using std::string;
using std::stoi;
using std::to_string;

int strToInt(string str) {
    int num;
    size_t pos;
    try{
        num = stoi(str, &pos);
        if (pos != str.size()) {
            throw ERR_EXPECTED_INTEGER;
        }
    } catch(...) {
        throw ERR_EXPECTED_INTEGER;
    }
    return num;
}

bool endsWith(string str, string expected) {
    if(str.size() < expected.size()) {
        return false;
    }
    int i = expected.size()-1, j = str.size()-1;
    while(i >= 0) {
        if(str[j] != expected[i]) {
            return false;
        }
        i--, j--;
    }
    return true;
}

#endif //strUtils