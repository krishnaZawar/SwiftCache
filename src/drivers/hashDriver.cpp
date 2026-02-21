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

        void assert(Object* obj){
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
                obj = new Object(new Key(key), new Value(datatype, new HashDatatype()));
                db->insertObject(obj);
            }
            else{
                db->assertKeyExists(key);
            }
            return (HashDatatype*)obj->getValue();
        }

        string HSet(vector<string> &tokens){
            HashDatatype* value = getValue(tokens[1], true);
            for(int i = 2; i < tokens.size(); i+=2){
                value->setField(tokens[i], tokens[i+1]);
            }
            return "Success: Field(s) set successfully in the Hash";
        }
        
        string HGet(vector<string> &tokens){
            HashDatatype* value = getValue(tokens[1]);
            string values = "";
            for(int i = 2; i < tokens.size(); i++){
                try{
                    values += value->getField(tokens[i])+'\n';
                }
                catch(string &err){
                    throw err + "\nError fetching "+tokens[i]+"\nValue(s) of first "+to_string(i-2)+" field(s) are:\n"+values;
                }
            }
            return "Value(s) are:\n"+values;
        }
        string HGetAll(vector<string> &tokens){
            HashDatatype* value = getValue(tokens[1]);
            vector<string> keyVals = value->getKeyVals();
            string keyValues = "";
            for(int i = 0; i < keyVals.size(); i+=2){
                keyValues += keyVals[i] + " : "+keyVals[i+1]+"\n";
            }
            return "Success: Key-value pairs are:\n"+keyValues;
        }
        string HKeys(vector<string> &tokens){
            HashDatatype* value = getValue(tokens[1]);
            vector<string> keyList = value->getKeys();
            string keys = "";
            for(auto &key: keyList){
                keys += key + "\n";
            }
            return "Success: The keys in the hash are:\n"+keys;
        }
        
        string HDel(vector<string> &tokens){
            HashDatatype* value = getValue(tokens[1]);
            for(int i = 2; i < tokens.size(); i++){
                try{
                    value->delField(tokens[i]);
                }
                catch(string &err){
                    throw err + "\nFirst "+to_string(i-2)+" field(s) are deleted";
                }
            }
            return "Success: The field(s) are deleted";
        }

        string HExists(vector<string> &tokens){
            HashDatatype* value = getValue(tokens[1]);
            string existsStr = "";
            for(int i = 2; i < tokens.size(); i++){
                existsStr += (value->fieldExists(tokens[i])? "true" : "false");
                existsStr += "\n";
            }
            return "The Result is:\n"+existsStr;
        }

        string HLen(vector<string> &tokens){
            HashDatatype* value = getValue(tokens[1]);
            return "The length of the hash is: "+to_string(value->len());
        }
        string HStrLen(vector<string> &tokens){
            HashDatatype* value = getValue(tokens[1]);
            string lengths = "";
            for(int i = 2; i < tokens.size(); i++){
                try{    
                    lengths += to_string(value->strLen(tokens[i]))+"\n";
                }
                catch(string &err){
                    throw err + "Error fetching field "+tokens[i]+"\nThe lengths of the first "+to_string(i-2)+" Field(s) are:\n"+lengths;
                }
            }
            return "The length of the field(s) are: \n"+lengths;
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
            if(tokens[0] == "HGET"){
                if(tokens.size() < 3){
                    throw string("Error: Invalid usage of HGET command");
                }
                return HGet(tokens);
            }
            if(tokens[0] == "HGETALL"){
                if(tokens.size() != 2){
                    throw string("Error: Invalid usage of HGETALL command");
                }
                return HGetAll(tokens);
            }
            if(tokens[0] == "HKEYS"){
                if(tokens.size() != 2){
                    throw string("Error: Invalid usage of HKEYS command");
                }
                return HKeys(tokens);
            }
            if(tokens[0] == "HDEL"){
                if(tokens.size() < 3){
                    throw string("Error: Invalid usage of HDEL command");
                }
                return HDel(tokens);
            }
            if(tokens[0] == "HEXISTS"){
                if(tokens.size() < 3){
                    throw string("Error: Invalid usage of HEXISTS command");
                }
                return HExists(tokens);
            }
            if(tokens[0] == "HLEN"){
                if(tokens.size() != 2){
                    throw string("Error: Invalid usage of HLEN command");
                }
                return HLen(tokens);
            }
            if(tokens[0] == "HSTRLEN"){
                if(tokens.size() < 3){
                    throw string("Error: Invalid usage of HSTRLEN command");
                }
                return HStrLen(tokens);
            }
            return "";
        }
};

#endif // StringDriver_class