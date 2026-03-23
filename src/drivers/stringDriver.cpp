#include<string>
#include<vector>
#include "../database/database.cpp"
#include "../models/models.h"

#ifndef StringDriver_class
#define StringDriver_class

using std::string;
using std::vector;
using std::to_string;

class StringDriver{
    private:
        char datatype = 's';
        Database *db;

        inline void assert(Object* obj){
            if(obj->getType() != datatype){
                throw string("Error: key type mismatch");
            }
        }

        string Set(vector<string> &tokens){
            Object* obj;
            for(int i = 1; i < tokens.size(); i+=2){
                obj = db->getObject(tokens[i]);
                if(obj != NULL){
                    try{
                        assert(obj);
                    }
                    catch(string &err){
                        throw err + "\nError inserting "+tokens[i];
                    }
                }
            }
            for(int i = 1; i < tokens.size(); i += 2){
                obj = db->getObject(tokens[i]);
                string* newValue = new string(tokens[i+1]);
                if(!obj){
                    db->insertObject(new Object(tokens[i], new Value(datatype, newValue)));
                }
                else{
                    obj->updateValue(newValue);
                }
            }
            return "1";
        }
        
        string Get(vector<string> &tokens){
            string values = "";
            Object* obj;
            for(int i = 1; i < tokens.size(); i++){
                try{
                    db->assertKeyExists(tokens[i]);
                    obj = db->getObject(tokens[i]);
                    assert(obj);
                }
                catch(string &err){
                    throw err + "\nError fetching "+tokens[i];
                }
                values += *(string*)(obj->getValue());
                values += "\n";
            }
            values.pop_back();
            return values;
        }

    public:
        StringDriver() {}
        StringDriver(Database *_db){
            db = _db;
        }

        string execDriver(vector<string> &tokens){
            db->runExpiryLoop();
            if(tokens[0] == "SET"){
                if(tokens.size() < 3 || tokens.size() % 2 == 0){
                    throw string("Error: Invalid usage of SET command");
                }
                return Set(tokens);
            }
            else if(tokens[0] == "GET"){
                if(tokens.size() < 2){
                    throw string("Error: Invalid usage of GET command");
                }
                return Get(tokens);
            }
            else{
                throw string("Error: Invalid Command");
            }
            return "";
        }
};

#endif // StringDriver_class