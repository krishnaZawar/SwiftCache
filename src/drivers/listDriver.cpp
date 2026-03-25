#include<string>
#include<vector>

#include "../constants/const.h"
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

        ListDatatype* getValue(string key){
            Object* obj = db->getObject(key);
            if(obj != NULL){
                assert(obj);
            }
            else{
                obj = new Object(key, new Value(datatype, new ListDatatype()));
                db->insertObject(obj);
            }
            return (ListDatatype*)obj->getValue();
        }
    
    public:
        inline string LPush(vector<string> &tokens){
            db->runExpiryLoop();
            assertSyntaxCheck(tokens.size() < 3, ERR_LPUSH_COMM);
            string key = tokens[1];
            ListDatatype* value = getValue(key);
            for(int i = 2; i < tokens.size(); i++){
                value->pushLeft(tokens[i]);
            }
            return "1";
        }
        inline string RPush(vector<string> &tokens){
            db->runExpiryLoop();
            assertSyntaxCheck(tokens.size() < 3, ERR_RPUSH_COMM);
            string key = tokens[1];
            ListDatatype* value = getValue(key);
            for(int i = 2; i < tokens.size(); i++){
                value->pushRight(tokens[i]);
            }
            return "1";
        }
        inline string LPushIdx(vector<string> &tokens){
            db->runExpiryLoop();
            assertSyntaxCheck(tokens.size() != 4, ERR_LPUSHIDX_COMM);
            ListDatatype* value = getValue(tokens[1]);
            value->pushAtIdx(stoi(tokens[3]), tokens[2]);
            return "1";
        }
        
        string LPop(vector<string> &tokens){
            db->runExpiryLoop();
            assertSyntaxCheck(tokens.size() != 3, ERR_LPOP_COMM);
            db->assertKeyExists(tokens[1]);
            ListDatatype* value = getValue(tokens[1]);
            int count = stoi(tokens[2]);
            if(count > value->len()){
                throw string("Error: pop count is greater than list size");
            }
            for(int i = 1; i <= count; i++){
                value->popLeft();
            }
            return "1";
        }
        string RPop(vector<string> &tokens){
            db->runExpiryLoop();
            assertSyntaxCheck(tokens.size() != 3, ERR_RPOP_COMM);
            db->assertKeyExists(tokens[1]);
            ListDatatype* value = getValue(tokens[1]);
            int count = stoi(tokens[2]);
            if(count > value->len()){
                throw string("Error: pop count is greater than list size");
            }
            for(int i = 1; i <= count; i++){
                value->popRight();
            }
            return "1";
        }
        inline string LPopIdx(vector<string> &tokens){
            db->runExpiryLoop();
            assertSyntaxCheck(tokens.size() != 3, ERR_LPOPIDX_COMM);
            db->assertKeyExists(tokens[1]);
            ListDatatype* value = getValue(tokens[1]);
            value->popAtIdx(stoi(tokens[2]));
            return "1";
        }
        
        inline string LLen(vector<string> &tokens){
            db->runExpiryLoop();
            assertSyntaxCheck(tokens.size() != 2, ERR_LLEN_COMM);
            db->assertKeyExists(tokens[1]);
            return to_string(getValue(tokens[1])->len());
        }
        string LIndex(vector<string> &tokens){
            db->runExpiryLoop();
            assertSyntaxCheck(tokens.size() < 3, ERR_LINDEX_COMM);
            db->assertKeyExists(tokens[1]);
            ListDatatype* value = getValue(tokens[1]);
            string values = "";
            for(int i = 2; i < tokens.size(); i++){
                values += value->getIdx(stoi(tokens[i])) + "\n";
            }
            values.pop_back();
            return values;
        }
        string LRange(vector<string> &tokens){
            db->runExpiryLoop();
            assertSyntaxCheck(tokens.size() != 4, ERR_LRANGE_COMM);
            db->assertKeyExists(tokens[1]);
            ListDatatype* value = getValue(tokens[1]);
            int start = stoi(tokens[2]);
            int end = stoi(tokens[3]);
            value->assertIdxBounds(start);
            value->assertIdxBounds(end);
            string returnVal = "";
            while(start <= end){
                returnVal += value->getIdx(start)+"\n";
                start++;
            }
            returnVal.pop_back();
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
            else if(tokens[0] == "RPUSH"){
                if(tokens.size() < 3){
                    throw string("Error: Invalid usage of RPUSH command");
                }
                return RPush(tokens);
            }
            else if(tokens[0] == "LPUSHIDX"){
                if(tokens.size() != 4){
                    throw string("Error: Invalid usage of LPUSHIDX command");
                }
                return LPushIdx(tokens);
            }
            else if(tokens[0] == "LPOP"){
                if(tokens.size() != 3){
                    throw string("Error: Invalid usage of LPOP command");
                }
                return LPop(tokens);
            }
            else if(tokens[0] == "RPOP"){
                if(tokens.size() != 3){
                    throw string("Error: Invalid usage of RPOP command");
                }
                return RPop(tokens);
            }
            else if(tokens[0] == "LPOPIDX"){
                if(tokens.size() != 3){
                    throw string("Error: Invalid usage of LPOPIDX command");
                }
                return LPopIdx(tokens);
            }
            else if(tokens[0] == "LLEN"){
                if(tokens.size() != 2){
                    throw string("Error: Invalid usage of LLEN command");
                }
                return LLen(tokens);
            }
            else if(tokens[0] == "LINDEX"){
                if(tokens.size() < 3){
                    throw string("Error: Invalid usage of LINDEX command");
                }
                return LIndex(tokens);
            }
            else if(tokens[0] == "LRANGE"){
                if(tokens.size() != 4){
                    throw string("Error: Invalid usage of LRANGE command");
                }
                return LRange(tokens);
            }
            else{
                throw string("Error: Invalid Command");
            }
            return "";
        }
};

#endif // ListDriver_class