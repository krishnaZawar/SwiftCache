#include<string>
#include<vector>

#include "../base/const.h"
#include "../database/database.cpp"
#include "../models/models.h"

#ifndef hashDriver_class
#define hashDriver_class

using std::string;
using std::vector;
using std::to_string;

class HashDriver{
    private:
        char datatype = 'h';
        Database *db;

        inline void assertSyntaxCheck(bool wrongSyntax, const string &errMsg) {
            if(wrongSyntax){
                throw errMsg;
            }
        }

        inline void assert(Object* obj){
            if(obj->getType() != datatype){
                throw string("Error: key type mismatch");
            }
        }
        HashDatatype* getValue(string key, bool insert = false){
            Object* obj = db->getObject(key);
            if(obj != NULL){
                assert(obj);
            }
            else if(insert){
                obj = new Object(key, new Value(datatype, new HashDatatype()));
                db->insertObject(obj);
            }
            else{
                db->assertKeyExists(key);
            }
            return (HashDatatype*)obj->getValue();
        }
        
    public:
        inline string HSet(vector<string> &tokens){
            db->runExpiryLoop();
            assertSyntaxCheck(tokens.size() < 4 || tokens.size() % 2 != 0, ERR_HSET_COMM);
            HashDatatype* value = getValue(tokens[1], true);
            for(int i = 2; i < tokens.size(); i+=2){
                value->setField(tokens[i], tokens[i+1]);
            }
            return "1";
        }
        
        string HGet(vector<string> &tokens){
            db->runExpiryLoop();
            assertSyntaxCheck(tokens.size() < 3, ERR_HGET_COMM);
            HashDatatype* value = getValue(tokens[1]);
            string values = "";
            for(int i = 2; i < tokens.size(); i++){
                try{
                    values += value->getField(tokens[i])+'\n';
                }
                catch(string &err){
                    throw err + "\nError fetching "+tokens[i];
                }
            }
            values.pop_back();
            return values;
        }
        string HGetAll(vector<string> &tokens){
            db->runExpiryLoop();
            assertSyntaxCheck(tokens.size() != 2, ERR_HGETALL_COMM);
            HashDatatype* value = getValue(tokens[1]);
            vector<string> keyVals = value->getKeyVals();
            string keyValues = "";
            for(int i = 0; i < keyVals.size(); i+=2){
                keyValues += keyVals[i] + " : "+keyVals[i+1]+"\n";
            }
            keyValues.pop_back();
            return keyValues;
        }
        string HKeys(vector<string> &tokens){
            db->runExpiryLoop();
            assertSyntaxCheck(tokens.size() != 2, ERR_HKEYS_COMM);
            HashDatatype* value = getValue(tokens[1]);
            vector<string> keyList = value->getKeys();
            string keys = "";
            for(auto &key: keyList){
                keys += key + " ";
            }
            keys.pop_back();
            return keys;
        }
        
        string HDel(vector<string> &tokens){
            db->runExpiryLoop();
            assertSyntaxCheck(tokens.size() < 3, ERR_HDEL_COMM);
            HashDatatype* value = getValue(tokens[1]);
            string res = "";
            for(int i = 2; i < tokens.size(); i++){
                try{
                    value->delField(tokens[i]);
                    res += "1";
                }
                catch(string &err){
                    res += "0";
                }
            }
            return res;
        }
        
        string HExists(vector<string> &tokens){
            db->runExpiryLoop();
            assertSyntaxCheck(tokens.size() < 3, ERR_HEXISTS_COMM);
            HashDatatype* value = getValue(tokens[1]);
            string existsStr = "";
            for(int i = 2; i < tokens.size(); i++){
                existsStr += (value->fieldExists(tokens[i])? "1" : "0");
            }
            return existsStr;
        }
        
        inline string HLen(vector<string> &tokens){
            db->runExpiryLoop();
            assertSyntaxCheck(tokens.size() != 2, ERR_HLEN_COMM);
            HashDatatype* value = getValue(tokens[1]);
            return to_string(value->len());
        }
        string HStrLen(vector<string> &tokens){
            db->runExpiryLoop();
            assertSyntaxCheck(tokens.size() < 3, ERR_HSTRLEN_COMM);
            HashDatatype* value = getValue(tokens[1]);
            string lengths = "";
            for(int i = 2; i < tokens.size(); i++){
                try{    
                    lengths += to_string(value->strLen(tokens[i]))+"\n";
                }
                catch(string &err){
                    throw err + "Error fetching field "+tokens[i];
                }
            }
            lengths.pop_back();
            return lengths;
        }

    public:
        HashDriver() {}
        HashDriver(Database *_db){
            db = _db;
        }
};

#endif // StringDriver_class