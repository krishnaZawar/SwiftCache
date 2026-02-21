#include<string>
#include<ctime>
#include "key.cpp"
#include "value.cpp"

#ifndef Object_class
#define Object_class

using std::string;
using std::time_t;
using std::time;

class Object{
    private:
        Key* key;
        Value* value;
        time_t expiresAt;
    public:
        Object() {}
        Object(Key* _key, Value* val){
            key = _key;
            value = val;
            expiresAt = 0;
        }

        void setTTL(int seconds){
            expiresAt = time(NULL) + seconds;
        }
        void clearTTL(){
            expiresAt = 0;
        }
        bool expired(){
            return expiresAt != 0 && expiresAt <= time(NULL);
        }
        
        string getKey(){
            return key->get();
        }
        
        char getType(){
            return value->getType();
        }

        void* getValue(){
            return value->getValue();
        }

        void updateValue(void* val){
            value->updateValue(val);
        }

        ~Object(){
            delete key;
            delete value;
        }
};

#endif // Key_class