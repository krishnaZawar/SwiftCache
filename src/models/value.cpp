#include<vector>
#include<string>

#include "datatypes/datatypes.h"

#ifndef Value_class
#define Value_class

using std::string;
using std::vector;

class Value{
    private:
        char type;
        
        void* value;

        void isValidType(char _type){
            if(_type != 's' && _type != 'l' && _type != 'h'){
                throw std::string("Error: Invalid type setting");
            }
        }

        void deleteValue(){
            if(type == 's'){
                delete (StringDatatype*)value;
            }
            if(type == 'l'){
                delete (ListDatatype*)value;
            }
            if(type == 'h'){
                delete (HashDatatype*)value;
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

        char getType(){
            return type;
        }

        void updateValue(void* val){
            deleteValue();
            value = val;
        }

        void* getValue(){
            return value;
        }
        

        ~Value(){
            deleteValue();
        }
};

#endif // Value_class