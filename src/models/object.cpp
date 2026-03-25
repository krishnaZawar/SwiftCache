#include<string>
#include<ctime>

#include "value.cpp"

#ifndef Object_class
#define Object_class

using std::string;
using std::time_t;
using std::time;

class Object{
    private:
        string key;
        Value* value;
        time_t expiresAt;
    public:
        Object() {}
        Object(string _key, Value* val){
            key = _key;
            value = val;
            expiresAt = 0;
        }

        inline void setTTL(int seconds){
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

        ~Object(){
            delete value;
        }
};

#endif // Key_class