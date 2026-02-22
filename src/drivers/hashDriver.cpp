#include<string>
#include<vector>
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

        inline string HSet(vector<string> &tokens){
            HashDatatype* value = getValue(tokens[1], true);
            for(int i = 2; i < tokens.size(); i+=2){
                value->setField(tokens[i], tokens[i+1]);
            }
            return "1";
        }
        
        string HGet(vector<string> &tokens){
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
            HashDatatype* value = getValue(tokens[1]);
            string existsStr = "";
            for(int i = 2; i < tokens.size(); i++){
                existsStr += (value->fieldExists(tokens[i])? "1" : "0");
                existsStr += "";
            }
            return existsStr;
        }

        inline string HLen(vector<string> &tokens){
            HashDatatype* value = getValue(tokens[1]);
            return to_string(value->len());
        }
        string HStrLen(vector<string> &tokens){
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

        string execDriver(vector<string> &tokens){
            db->runExpiryLoop();
            if(tokens[0] == "HSET"){
                if(tokens.size() < 4 || tokens.size() % 2 != 0){
                    throw string("Error: Invalid usage of HSET command");
                }
                return HSet(tokens);
            }
            else if(tokens[0] == "HGET"){
                if(tokens.size() < 3){
                    throw string("Error: Invalid usage of HGET command");
                }
                return HGet(tokens);
            }
            else if(tokens[0] == "HGETALL"){
                if(tokens.size() != 2){
                    throw string("Error: Invalid usage of HGETALL command");
                }
                return HGetAll(tokens);
            }
            else if(tokens[0] == "HKEYS"){
                if(tokens.size() != 2){
                    throw string("Error: Invalid usage of HKEYS command");
                }
                return HKeys(tokens);
            }
            else if(tokens[0] == "HDEL"){
                if(tokens.size() < 3){
                    throw string("Error: Invalid usage of HDEL command");
                }
                return HDel(tokens);
            }
            else if(tokens[0] == "HEXISTS"){
                if(tokens.size() < 3){
                    throw string("Error: Invalid usage of HEXISTS command");
                }
                return HExists(tokens);
            }
            else if(tokens[0] == "HLEN"){
                if(tokens.size() != 2){
                    throw string("Error: Invalid usage of HLEN command");
                }
                return HLen(tokens);
            }
            else if(tokens[0] == "HSTRLEN"){
                if(tokens.size() < 3){
                    throw string("Error: Invalid usage of HSTRLEN command");
                }
                return HStrLen(tokens);
            }
            else{
                throw string("Error: Invalid Command");
            }
            return "";
        }
};

#endif // StringDriver_class