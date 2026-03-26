#include<string>
#include<vector>

#include "../base/const.h"
#include "../database/database.cpp"
#include "../models/models.h"
#include "../utils/utils.h"

#ifndef ListDriver_class
#define ListDriver_class

using std::string;
using std::vector;
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
                throw ERR_TYPE_MISMATCH;
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
            try{
                value->pushAtIdx(strToInt(tokens[3]), tokens[2]);
            } 
            catch(string& err) {
                throw err;
            }
            return "1";
        }
        
        string LPop(vector<string> &tokens){
            db->runExpiryLoop();
            assertSyntaxCheck(tokens.size() != 3, ERR_LPOP_COMM);
            db->assertKeyExists(tokens[1]);
            ListDatatype* value = getValue(tokens[1]);
            int count = strToInt(tokens[2]);
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
            int count = strToInt(tokens[2]);
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
            try{
                value->popAtIdx(strToInt(tokens[2]));
            }
            catch(string &err) {
                throw err;
            }
            catch(...) {
                throw ERR_EXPECTED_INTEGER;
            }
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
                try{
                    values += value->getIdx(strToInt(tokens[i])) + "\n";
                } 
                catch(string &err) {
                    throw err;
                }
                catch(...) {
                    throw ERR_EXPECTED_INTEGER;
                }
            }
            values.pop_back();
            return values;
        }
        string LRange(vector<string> &tokens){
            db->runExpiryLoop();
            assertSyntaxCheck(tokens.size() != 4, ERR_LRANGE_COMM);
            db->assertKeyExists(tokens[1]);
            ListDatatype* value = getValue(tokens[1]);
            int start = strToInt(tokens[2]);
            int end = strToInt(tokens[3]);
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
};

#endif // ListDriver_class