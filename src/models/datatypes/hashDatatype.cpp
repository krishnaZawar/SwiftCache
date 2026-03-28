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

        inline void assertFieldExists(string field){
            if(!fieldExists(field)){
                throw ERR_HASH_FIELD_DOES_NOT_EXIST;
            }
        }
    public:
        HashDatatype(){}
        
        inline bool fieldExists(string field){
            return hash.find(field) != hash.end();
        }

        inline void setField(string field, string value){
            hash[field] = value;
        }
        
        inline string getField(string field){
            assertFieldExists(field);
            return hash[field];
        }
        
        inline vector<string> getKeys(){
            vector<string> keys;
            for(auto &[key, _]: hash){
                keys.push_back(key);
            }
            return keys;
        }

        inline vector<string> getKeyVals(){
            vector<string> keyVals;
            for(auto &[key, val]: hash){
                keyVals.push_back(key);
                keyVals.push_back(val);
            }
            return keyVals;
        }
        
        inline void delField(string field){
            assertFieldExists(field);
            hash.erase(field);
        }

        inline int len(){
            return hash.size();
        }

        inline int strLen(string field){
            assertFieldExists(field);
            return hash[field].size();
        }
};

#endif // hashDatatype_class