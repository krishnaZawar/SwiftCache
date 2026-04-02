#include<vector>
#include<string>

#include "datatypes/datatypes.h"
#include "../base/const.h"

#ifndef Value_class
#define Value_class

using std::string;
using std::vector;

class Value{
    private:
        char type;
        
        void* value;

        inline void isValidType(char _type){
            if(_type != 's' && _type != 'l' && _type != 'h'){
                throw std::string("Error: Invalid type setting");
            }
            switch(_type) {
                case STRING_DATATYPE:
                case LIST_DATATYPE:
                case HASH_DATATYPE:
                    break;
                default:
                    throw std::string("Error: Invalid type setting");
            }
        }

        inline void deleteValue(){
            switch(type) {
                case STRING_DATATYPE:
                    delete (string*)value;
                    break;
                case LIST_DATATYPE:
                    delete (ListDatatype*)value;
                    break;
                case HASH_DATATYPE:
                    delete (HashDatatype*)value;
                    break;
            }
        }
    public:
        Value(){
            value = NULL;
        }
        Value(char _type){
            isValidType(_type);
            type = _type;
            value = NULL;
        }
        Value(char _type, void* val){
            isValidType(_type);
            type = _type;
            value = val;
        }

        inline char getType(){
            return type;
        }

        inline void updateValue(void* val){
            deleteValue();
            value = val;
        }

        inline void* getValue(){
            return value;
        }
        

        ~Value(){
            deleteValue();
        }
};

#endif // Value_class