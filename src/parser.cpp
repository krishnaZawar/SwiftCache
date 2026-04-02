#include<string>
#include<vector>
#include<iostream>
#include<unordered_map>

#include "base/errors.h"
#include "drivers/drivers.h"
#include "WAL/wal.cpp"

#ifndef Parser_class
#define Parser_class

using std::string;
using std::vector;
using std::unordered_map;

class Parser{
    private:
        unordered_map<string, int> commandMp;

        vector<string> string_commands;
        vector<string> generic_commands;
        vector<string> list_commands;
        vector<string> hash_commands;
        
        StringDriver stringDriver;
        ListDriver listDriver;
        GenericsDriver genericsDriver;
        HashDriver hashDriver;

        WAL *wal;
        bool captureLogs;

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

        void Constructor(Database *db) {
            /*
                These are not used anywhere.
                The command lists are kept just for reference and will be removed later.
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

            commandMp = unordered_map<string, int> ();

            // string commands
            commandMp["SET"] = COMM_SET;
            commandMp["GET"] = COMM_GET;

            // generic commands
            commandMp["DEL"] = COMM_DEL;
            commandMp["TYPE"] = COMM_TYPE;
            commandMp["EXPIRE"] = COMM_EXPIRE;
            commandMp["PERSIST"] = COMM_PERSIST;

            // list commands
            commandMp["LPUSH"] = COMM_LPUSH;
            commandMp["RPUSH"] = COMM_RPUSH;
            commandMp["LPUSHIDX"] = COMM_LPUSHIDX;
            commandMp["LPOP"] = COMM_LPOP;
            commandMp["RPOP"] = COMM_RPOP;
            commandMp["LPOPIDX"] = COMM_LPOPIDX;
            commandMp["LLEN"] = COMM_LLEN;
            commandMp["LINDEX"] = COMM_LINDEX;
            commandMp["LRANGE"] = COMM_LRANGE;

            // hash commands
            commandMp["HSET"] = COMM_HSET;
            commandMp["HGET"] = COMM_HGET;
            commandMp["HGETALL"] = COMM_HGETALL;
            commandMp["HKEYS"] = COMM_HKEYS;
            commandMp["HDEL"] = COMM_HDEL;
            commandMp["HEXISTS"] = COMM_HEXISTS;
            commandMp["HLEN"] = COMM_HLEN;
            commandMp["HSTRLEN"] = COMM_HSTRLEN;

            stringDriver = StringDriver(db);
            genericsDriver = GenericsDriver(db);
            listDriver = ListDriver(db);
            hashDriver = HashDriver(db);
        }

    public:
        Parser(Database *db) {
            Constructor(db);

            wal = NULL;
            captureLogs = false;
        }
        Parser(Database *db, WAL *wal) {
            Constructor(db);

            this->wal = wal;
            this->captureLogs = true;
        }

        void addWAL(WAL *wal) {
            this->wal = wal;
            this->captureLogs = true;
        }

        string parseCommand(const char* command, int size, bool dbInitialization = false){
            vector<string> tokens = tokenize(command, size);
            if(tokens.size() == 0){
                return "";
            }
            if(!commandMp.count(tokens[0])) {
                throw ERR_NO_COMM;
            }
            string result = "";
            switch(commandMp[tokens[0]]) {
                // string commands
                case COMM_SET:
                    result = stringDriver.Set(tokens);
                    if(captureLogs && !dbInitialization){
                        wal->appendLog(command);
                    }
                    break;
                case COMM_GET:
                    return stringDriver.Get(tokens);

                // generic commands
                case COMM_DEL:
                    if(captureLogs && !dbInitialization){
                        wal->appendLog(command);
                    }
                    return genericsDriver.Del(tokens);
                case COMM_TYPE:
                    return genericsDriver.Type(tokens);
                case COMM_EXPIRE:
                    if(captureLogs && !dbInitialization){
                        wal->appendLog(command);
                    }
                    return genericsDriver.Expire(tokens);
                case COMM_PERSIST:
                    if(captureLogs && !dbInitialization){
                        wal->appendLog(command);
                    }
                    return genericsDriver.Persist(tokens);

                // list commands
                case COMM_LPUSH:
                    result = listDriver.LPush(tokens);
                    if(captureLogs && !dbInitialization){
                        wal->appendLog(command);
                    }
                    break;
                case COMM_RPUSH:
                    result = listDriver.RPush(tokens);
                    if(captureLogs && !dbInitialization){
                        wal->appendLog(command);
                    }
                    break;
                case COMM_LPUSHIDX:
                    result = listDriver.LPushIdx(tokens);
                    if(captureLogs && !dbInitialization){
                        wal->appendLog(command);
                    }
                    break;
                case COMM_LPOP:
                    result = listDriver.LPop(tokens);
                    if(captureLogs && !dbInitialization){
                        wal->appendLog(command);
                    }
                    break;
                case COMM_RPOP:
                    result = listDriver.RPop(tokens);
                    if(captureLogs && !dbInitialization){
                        wal->appendLog(command);
                    }
                    break;
                case COMM_LPOPIDX:
                    result = listDriver.LPopIdx(tokens);
                    if(captureLogs && !dbInitialization){
                        wal->appendLog(command);
                    }
                    break;
                case COMM_LLEN:
                    return listDriver.LLen(tokens);
                case COMM_LINDEX:
                    return listDriver.LIndex(tokens);
                case COMM_LRANGE:
                    return listDriver.LRange(tokens);

                // hash commands
                case COMM_HSET:
                    result = hashDriver.HSet(tokens);
                    if(captureLogs && !dbInitialization){
                        wal->appendLog(command);
                    }
                    break;
                case COMM_HGET:
                    return hashDriver.HGet(tokens);
                case COMM_HGETALL:
                    return hashDriver.HGetAll(tokens);
                case COMM_HKEYS:
                    return hashDriver.HKeys(tokens);
                case COMM_HDEL:
                    result = hashDriver.HDel(tokens);
                    if(captureLogs && !dbInitialization){
                        wal->appendLog(command);
                    }
                    break;
                case COMM_HEXISTS:
                    return hashDriver.HExists(tokens);
                case COMM_HLEN:
                    return hashDriver.HLen(tokens);
                case COMM_HSTRLEN:
                    return hashDriver.HStrLen(tokens);
            }
            return result;
        }
};

#endif // Parser_class