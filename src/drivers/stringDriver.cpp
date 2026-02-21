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

        void assert(Object* obj){
            if(obj->getType() != datatype){
                throw string("Error: key type mismatch");
            }
        }

        string Set(vector<string> &tokens){
            for(int i = 1; i < tokens.size(); i+=2){
                Object* obj = db->getObject(tokens[i]);
                StringDatatype* newValue = new StringDatatype(tokens[i+1]);
                if(obj != NULL){
                    try{
                        assert(obj);
                    }
                    catch(string &err){
                        throw err + "\nError inserting "+tokens[i]+"\nFirst " + to_string(i/2) + " key(s) set successfully";
                    }
                    obj->updateValue(newValue);
                }
                else{
                    db->insertObject(new Object(new Key(tokens[i]), new Value(datatype, newValue)));
                }
            }
            return "Success: Key(s) set successfully";
        }
        
        string Get(vector<string> &tokens){
            string values = "";
            Object* obj;
            StringDatatype* value;
            for(int i = 1; i < tokens.size(); i++){
                try{
                    db->assertKeyExists(tokens[i]);
                    obj = db->getObject(tokens[i]);
                    assert(obj);
                }
                catch(string &err){
                    throw err + "\nError fetching "+tokens[i]+"\nValue(s) of first "+to_string(i-1)+" key(s) are:\n"+values;
                }
                value = (StringDatatype*)(obj->getValue());
                values += value->Get() + "\n";
            }
            return "Value(s) are:\n"+values;
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
            if(tokens[0] == "GET"){
                if(tokens.size() < 2){
                    throw string("Error: Invalid usage of GET command");
                }
                return Get(tokens);
            }
            return "";
        }
};

#endif // StringDriver_class