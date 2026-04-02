#include<string>
#include<vector>

#include "../base/errors.h"
#include "../database/database.cpp"
#include "../utils/utils.h"

#ifndef GenericsDriver_class
#define GenericsDriver_class

using std::string;
using std::vector;
using std::to_string;

class GenericsDriver{
    private:
        Database *db;

        inline void assertSyntaxCheck(bool wrongSyntax, const string &errMsg) {
            if(wrongSyntax){
                throw errMsg;
            }
        }

    public:
        string Type(vector<string> &tokens){
            db->runExpiryLoop();
            assertSyntaxCheck(tokens.size() < 2, ERR_TYPE_COMM);
            string res = "";
            for(int i = 1; i < tokens.size(); i++){
                try{
                    res += db->getType(tokens[i]) +" ";
                } catch(string &err){
                    res += "nil ";
                }
            }
            res.pop_back();
            return res;
        }
        
        string Del(vector<string> &tokens){
            db->runExpiryLoop();
            assertSyntaxCheck(tokens.size() < 2, ERR_DEL_COMM);
            string res = "";
            for(int i = 1; i < tokens.size(); i++){
                try{
                    db->deleteKey(tokens[i]);
                    res += "1";
                }
                catch(string &err){
                    res += "0";
                }
            }
            return res;
        }
        
        string Expire(vector<string> &tokens){
            db->runExpiryLoop();
            assertSyntaxCheck(tokens.size() < 3 || tokens.size() % 2 == 0, ERR_EXPIRE_COMM);
            string res = "";
            int expiry;
            for(int i = 1; i < tokens.size(); i+=2){
                try{
                    expiry = strToInt(tokens[i+1]);
                    if(expiry <= 0){
                        res += "0";
                    }
                    else{
                        db->assertKeyExists(tokens[i]);
                        db->getObject(tokens[i])->setTTL(expiry);
                        res += "1";
                    }
                }
                catch(string &err){
                    res += "0";
                }
                catch(...){
                    res += "0";
                }
            }
            return res;
        }
        
        string Persist(vector<string> &tokens){
            db->runExpiryLoop();
            assertSyntaxCheck(tokens.size() < 2, ERR_PERSIST_COMM);
            string res = "";
            for(int i = 1; i < tokens.size(); i++){
                try{
                    db->assertKeyExists(tokens[i]);
                    db->getObject(tokens[i])->clearTTL();
                    res += "1";
                }
                catch(string &err){
                    res += "0";
                }
            }
            return res;
        }

    public:
        GenericsDriver() {}
        GenericsDriver(Database *_db){
            db = _db;
        }
};

#endif // GenericsDriver_class