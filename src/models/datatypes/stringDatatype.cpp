#include<string>

#ifndef StringDatatype_class
#define StringDatatype_class

using std::string;

class StringDatatype{
    private:
        string value;
    public:
        StringDatatype(){}

        StringDatatype(string _value){
            value = _value;
        }
        string Get(){
            return value;
        }
        void Set(string val){
            value = val;
        }
};

#endif // StringDatatype_class