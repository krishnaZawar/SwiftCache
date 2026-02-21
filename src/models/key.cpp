#include<string>

#ifndef Key_class
#define Key_class

using std::string;

class Key{
    private:
        string key;
    public:
        Key() {}
        Key(string _key){
            key = _key;
        }
        string get(){
            return key;
        }
};

#endif // Key_class