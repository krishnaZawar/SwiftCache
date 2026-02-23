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
        /*
            unused currently, might be required later
        */
        template<class t>
        bool existsIn(vector<t> &arr, t val){
            for(auto &_val: arr){
                if(val == _val){
                    return true;
                }
            }
            return false;
        }
        
        vector<string> tokenize(const char* command, int size){
            vector<string> tokens;
            string cur = "";
            bool quote_open = false;
            for(int i = 0; i < size; i++){
                if(quote_open){
                    if(command[i] == '\"'){
                        tokens.push_back(cur);
                        cur = "";
                        quote_open = false;
                    }
                    else{
                        cur += command[i];
                    }
                } else if(command[i] == ' '){
                    if(cur != ""){
                        tokens.push_back(cur);
                    }
                    cur = "";
                } else if(command[i] == '\"'){
                    quote_open = true;
                } else{
                    cur += command[i];
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
            /*
                These are not used anywhere.
                The commands are kept just for reference and will be removed later.
            */
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

        string parseCommand(const char* command, int size){
            vector<string> tokens = tokenize(command, size);
            if(tokens.size() == 0){
                return "";
            }
            switch(tokens[0][0]){
                case 'S':
                case 'G':
                    return stringDriver.execDriver(tokens);
                    break;
                case 'L':
                case 'R':
                    return listDriver.execDriver(tokens);
                    break;
                case 'H':
                    return hashDriver.execDriver(tokens);
                    break;
                case 'D':
                case 'T':
                case 'E':
                case 'P':
                    return genericsDriver.execDriver(tokens);
                    break;
                default:
                    throw string("Error: Invalid Command");
            }
            return "";
        }
};

#endif // Parser_class