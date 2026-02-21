#include<string>
#include<unordered_map>
#include<vector>

#ifndef hashDatatype_class
#define hashDatatype_class

using std::string;
using std::unordered_map;
using std::vector;

class HashDatatype{
    private:
        unordered_map<string, string> hash;

        void assertFieldExists(string field){
            if(!fieldExists(field)){
                throw string("Error: field does not exist");
            }
        }
    public:
        HashDatatype(){}
        
        bool fieldExists(string field){
            return hash.find(field) != hash.end();
        }

        void setField(string field, string value){
            hash[field] = value;
        }
        
        string getField(string field){
            assertFieldExists(field);
            return hash[field];
        }
        
        vector<string> getKeys(){
            vector<string> keys;
            for(auto &[key, _]: hash){
                keys.push_back(key);
            }
            return keys;
        }

        vector<string> getKeyVals(){
            vector<string> keyVals;
            for(auto &[key, val]: hash){
                keyVals.push_back(key);
                keyVals.push_back(val);
            }
            return keyVals;
        }
        
        void delField(string field){
            assertFieldExists(field);
            hash.erase(field);
        }

        int len(){
            return hash.size();
        }

        int strLen(string field){
            assertFieldExists(field);
            return hash[field].size();
        }
};

#endif // hashDatatype_class