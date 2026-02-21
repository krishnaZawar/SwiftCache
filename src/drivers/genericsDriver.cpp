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
            string res = "Type of the key(s): \n";
            for(int i = 1; i < tokens.size(); i++){
                res += db->getType(tokens[i]) +"\n";
            }
            return res;
        }
        
        string Del(vector<string> &tokens){
            for(int i = 1; i < tokens.size(); i++){
                db->deleteKey(tokens[i]);
            }
            return "Success: key(s) deleted successfully";;
        }
        
        string Expire(vector<string> &tokens){
            for(int i = 1; i < tokens.size(); i+=2){
                db->assertKeyExists(tokens[i]);
                db->getObject(tokens[i])->setTTL(stoi(tokens[i+1]));
            }
            return "Success: Expiry set to the key(s)";
        }
        
        string Persist(vector<string> &tokens){
            for(int i = 1; i < tokens.size(); i++){
                db->assertKeyExists(tokens[i]);
                db->getObject(tokens[i])->clearTTL();
            }
            return "Success: TTL cleared for the key(s)";
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
            if(tokens[0] == "PERSIST"){
                if(tokens.size() < 2){
                    throw string("Error: Invalid usage of PERSIST command");
                }
                return Persist(tokens);
            }
            return "";
        }
};

#endif // GenericsDriver_class