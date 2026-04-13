#include<string>
#include<ctime>
#include<vector>

#include "value.cpp"
#include "../base/const.h"

#ifndef Object_class
#define Object_class

using std::string;
using std::to_string;
using std::time_t;
using std::time;

class Object{
    private:
        string key;
        Value* value;
        int expireSeconds;
        time_t expiresAt;
    public:
        Object() {}
        Object(string _key, Value* val){
            key = _key;
            value = val;
            expiresAt = 0;
            expireSeconds = 0;
        }

        inline void setTTL(int seconds){
            expireSeconds = seconds;
            expiresAt = time(NULL) + seconds;
        }
        inline void clearTTL(){
            expiresAt = 0;
        }
        inline bool expired(){
            return expiresAt != 0 && expiresAt <= time(NULL);
        }
        
        inline string getKey(){
            return key;
        }
        
        inline char getType(){
            return value->getType();
        }

        inline void* getValue(){
            return value->getValue();
        }

        inline void updateValue(void* val){
            value->updateValue(val);
        }

        vector<string> buildCommands() {
            vector<string> commands;
            switch(getType()) {
                case STRING_DATATYPE:
                    commands.push_back("SET "+key+" "+*((string*)getValue()));
                    break;
                case LIST_DATATYPE:
                    commands = ((ListDatatype*)value->getValue())->buildCommands(key);
                    break;
                case HASH_DATATYPE:
                    commands.push_back(((HashDatatype*)value->getValue())->buildCommand(key));
                    break;
            }
            if(expireSeconds != 0) {
                commands.push_back("EXPIRE "+key+" "+to_string(expireSeconds));
            }
            return commands;
        }

        ~Object(){
            delete value;
        }
};

#endif // Key_class