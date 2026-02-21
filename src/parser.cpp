#include<string>
#include<vector>
#include<iostream>

#include "drivers/drivers.h"

#ifndef Parser_class
#define Parser_class

using std::string;
using std::vector;

class Parser{
    private:
        template<class t>
        bool existsIn(vector<t> &arr, t val){
            for(auto &_val: arr){
                if(val == _val){
                    return true;
                }
            }
            return false;
        }
        
        vector<string> tokenize(string command){
            vector<string> tokens;
            string cur = "";
            bool quote_open = false;
            for(auto &ch : command){
                if(quote_open){
                    if(ch == '\"'){
                        tokens.push_back(cur);
                        cur = "";
                        quote_open = false;
                    }
                    else{
                        cur += ch;
                    }
                } else if(ch == ' '){
                    tokens.push_back(cur);
                    cur = "";
                } else if(ch == '\"'){
                    quote_open = true;
                } else{
                    cur += ch;
                }
            }
            if(cur != ""){
                tokens.push_back(cur);
            }
            
            if(quote_open){
                throw string("Error: Missing double quotes");
            }
            return tokens;
        }

        vector<string> string_commands;
        vector<string> generic_commands;
        vector<string> list_commands;
        vector<string> hash_commands;
        
        StringDriver stringDriver;
        ListDriver listDriver;
        GenericsDriver genericsDriver;
        HashDriver hashDriver;

    public:
        Parser(Database *db) {
            string_commands = vector<string> {
                "SET", "GET"
            };
            generic_commands = vector<string> {
                "DEL", "TYPE", "EXPIRE", "PERSIST"
            };
            list_commands = vector<string> {
                "LPUSH", "RPUSH", "LPUSHIDX", "LPOP", "RPOP", "LPOPIDX", "LLEN", "LINDEX", "LRANGE"  
            };
            hash_commands = vector<string> {
                "HSET", "HGET", "HGETALL", "HKEYS", "HDEL", "HEXISTS", "HLEN", "HSTRLEN"
            };

            stringDriver = StringDriver(db);
            genericsDriver = GenericsDriver(db);
            listDriver = ListDriver(db);
            hashDriver = HashDriver(db);
        }

        string parseCommand(string command){
            vector<string> tokens = tokenize(command);
            if(tokens.size() == 0){
                return "";
            }
            if(existsIn(string_commands, tokens[0])) {
                return stringDriver.execDriver(tokens);
            }
            else if(existsIn(generic_commands, tokens[0])){
                return genericsDriver.execDriver(tokens);
            }
            else if(existsIn(list_commands, tokens[0])){
                return listDriver.execDriver(tokens);
            }
            else if(existsIn(hash_commands, tokens[0])){
                return hashDriver.execDriver(tokens);
            }
            else {
                throw string("Error: Invalid Command");
            }
            return "";
        }
};

#endif // Parser_class