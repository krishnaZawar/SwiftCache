#include<string>
#include<vector>
#include "../database/database.cpp"

#ifndef GenericsDriver_class
#define GenericsDriver_class

using std::string;
using std::vector;
using std::stoi;
using std::to_string;

class GenericsDriver{
    private:
        Database *db;

        string Type(vector<string> &tokens){
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
            string res = "";
            int expiry;
            for(int i = 1; i < tokens.size(); i+=2){
                try{
                    expiry = stoi(tokens[i+1]);
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
            }
            return res;
        }
        
        string Persist(vector<string> &tokens){
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

        string execDriver(vector<string> &tokens){
            db->runExpiryLoop();
            if(tokens[0] == "DEL"){
                if(tokens.size() < 2){
                    throw string("Error: Invalid usage of DEL command");
                }
                return Del(tokens);
            }
            else if(tokens[0] == "TYPE"){
                if(tokens.size() < 2){
                    throw string("Error: Invalid usage of TYPE command");
                }
                return Type(tokens); 
            }
            else if(tokens[0] == "EXPIRE"){
                if(tokens.size() < 3 || tokens.size() % 2 == 0){
                    throw string("Error: Invalid usage of EXPIRE command");
                }
                return Expire(tokens);
            }
            else if(tokens[0] == "PERSIST"){
                if(tokens.size() < 2){
                    throw string("Error: Invalid usage of PERSIST command");
                }
                return Persist(tokens);
            }
            else{
                throw string("Error: Invalid Command");
            }
            return "";
        }
};

#endif // GenericsDriver_class