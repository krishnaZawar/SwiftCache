#include<string>

#include "../base/const.h"

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

#endif //strUtils