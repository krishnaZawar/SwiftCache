#include<string>
#include<vector>

#include "../base/errors.h"
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
                throw ERR_TYPE_MISMATCH;
            }
        }

        inline void assertSyntaxCheck(bool wrongSyntax, const string &errMsg) {
            if(wrongSyntax){
                throw errMsg;
            }
        }

    public:
        string Set(vector<string> &tokens){
            db->runExpiryLoop();
            assertSyntaxCheck(tokens.size() < 3 || tokens.size() % 2 == 0, ERR_SET_COMM);
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
            db->runExpiryLoop();
            assertSyntaxCheck(tokens.size() < 2, ERR_GET_COMM);
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
};

#endif // StringDriver_class