#include<string>
#include<vector>
#include "../database/database.cpp"
#include "../models/models.h"

#ifndef ListDriver_class
#define ListDriver_class

using std::string;
using std::vector;
using std::stoi;
using std::to_string;

class ListDriver{
    private:
        char datatype = 'l';
        Database *db;

        void assert(Object* obj){
            if(obj->getType() != datatype){
                throw string("Error: key type mismatch");
            }
        }

        ListDatatype* getValue(string key){
            Object* obj = db->getObject(key);
            if(obj != NULL){
                assert(obj);
            }
            else{
                obj = new Object(new Key(key), new Value(datatype, new ListDatatype()));
                db->insertObject(obj);
            }
            return (ListDatatype*)obj->getValue();
        }

        string LPush(vector<string> &tokens){
            string key = tokens[1];
            ListDatatype* value = getValue(key);
            for(int i = 2; i < tokens.size(); i++){
                value->pushLeft(tokens[i]);
            }

            return "Success: Values added to the list successfully";
        }
        string RPush(vector<string> &tokens){
            string key = tokens[1];
            ListDatatype* value = getValue(key);
            for(int i = 2; i < tokens.size(); i++){
                value->pushRight(tokens[i]);
            }

            return "Success: Values added to the list successfully";
        }
        string LPushIdx(vector<string> &tokens){
            ListDatatype* value = getValue(tokens[1]);
            value->pushAtIdx(stoi(tokens[3]), tokens[2]);
            return "Success: Value pushed at the given index successfully";
        }

        string LPop(vector<string> &tokens){
            db->assertKeyExists(tokens[1]);
            ListDatatype* value = getValue(tokens[1]);
            int count = stoi(tokens[2]);
            if(count > value->len()){
                throw string("Error: pop count is greater than list size");
            }
            for(int i = 1; i <= count; i++){
                value->popLeft();
            }
            return "Success: value popped from the list successfully";
        }
        string RPop(vector<string> &tokens){
            db->assertKeyExists(tokens[1]);
            ListDatatype* value = getValue(tokens[1]);
            int count = stoi(tokens[2]);
            if(count > value->len()){
                throw string("Error: pop count is greater than list size");
            }
            for(int i = 1; i <= count; i++){
                value->popRight();
            }
            return "Success: value popped from the list successfully";
        }
        string LPopIdx(vector<string> &tokens){
            db->assertKeyExists(tokens[1]);
            ListDatatype* value = getValue(tokens[1]);
            value->popAtIdx(stoi(tokens[2]));
            return "Success: value popped from the list successfully";
        }

        string LLen(vector<string> &tokens){
            db->assertKeyExists(tokens[1]);
            return "Success: Length of the list is " + to_string(getValue(tokens[1])->len());
        }
        string LIndex(vector<string> &tokens){
            db->assertKeyExists(tokens[1]);
            ListDatatype* value = getValue(tokens[1]);
            string values = "";
            for(int i = 2; i < tokens.size(); i++){
                try{
                    values += value->getIdx(stoi(tokens[i])) + "\n";
                }
                catch(string &err){
                    throw err + "Values at the given first "+to_string(i-2)+" index(s) are:\n" + values;
                }
            }
            return "Success: Value(s) are: \n" + values;
        }
        string LRange(vector<string> &tokens){
            db->assertKeyExists(tokens[1]);
            ListDatatype* value = getValue(tokens[1]);
            int start = stoi(tokens[2]);
            int end = stoi(tokens[3]);
            value->assertIdxBounds(start);
            value->assertIdxBounds(end);
            string returnVal = "Success: The values are\n";
            while(start <= end){
                returnVal += value->getIdx(start)+"\n";
                start++;
            }
            return returnVal;
        }

    public:
        ListDriver() {}
        ListDriver(Database *_db){
            db = _db;
        }

        string execDriver(vector<string> &tokens){
            db->runExpiryLoop();
            if(tokens[0] == "LPUSH"){
                if(tokens.size() < 3){
                    throw string("Error: Invalid usage of LPUSH command");
                }
                return LPush(tokens);
            }
            if(tokens[0] == "RPUSH"){
                if(tokens.size() < 3){
                    throw string("Error: Invalid usage of RPUSH command");
                }
                return RPush(tokens);
            }
            if(tokens[0] == "LPUSHIDX"){
                if(tokens.size() != 4){
                    throw string("Error: Invalid usage of LPUSHIDX command");
                }
                return LPushIdx(tokens);
            }
            if(tokens[0] == "LPOP"){
                if(tokens.size() != 3){
                    throw string("Error: Invalid usage of LPOP command");
                }
                return LPop(tokens);
            }
            if(tokens[0] == "RPOP"){
                if(tokens.size() != 3){
                    throw string("Error: Invalid usage of RPOP command");
                }
                return RPop(tokens);
            }
            if(tokens[0] == "LPOPIDX"){
                if(tokens.size() != 3){
                    throw string("Error: Invalid usage of LPOPIDX command");
                }
                return LPopIdx(tokens);
            }
            if(tokens[0] == "LLEN"){
                if(tokens.size() != 2){
                    throw string("Error: Invalid usage of LLEN command");
                }
                return LLen(tokens);
            }
            if(tokens[0] == "LINDEX"){
                if(tokens.size() < 3){
                    throw string("Error: Invalid usage of LINDEX command");
                }
                return LIndex(tokens);
            }
            if(tokens[0] == "LRANGE"){
                if(tokens.size() != 4){
                    throw string("Error: Invalid usage of LRANGE command");
                }
                return LRange(tokens);
            }
            return "";
        }
};

#endif // ListDriver_class