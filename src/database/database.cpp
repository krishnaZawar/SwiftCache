#include<vector>
#include<string>
#include<ctime>
#include<iostream>

#include "../models/models.h"
#include "../base/errors.h"
#include "../base/const.h"
#include "../WAL/wal.cpp"

#ifndef Database_class
#define Database_class

using std::string;
using std::cout;
using std::endl;
using std::swap;

class Database{
    private:
        const int minTableSize = (1 << 2)+1;
        const int maxTableSize = (1 << 25)+1;
        const float loadFactorToScaleUp = 1;
        const float loadFactorToScaleDown = 0.25;
        const int rehashBatchSize = 10;
        const int expiryBatchSize = 10;
        const int compactionBatchSize = 10;

        vector<vector<Object*>> primaryDb;
        vector<vector<Object*>> rehashDb;
        
        private:
        int primaryDbIdxForExpiry;
        
        float keysPresent;
        int primaryDbSize;
        
        int primaryDbIdxForRehash;
        int rehashDbSize;
        bool underRehash;

        WAL *wal;
        bool walActive;
        int compactionRowPrimary;
        int compactionRowRehash;

        inline bool underCompaction() {
            return walActive && wal->isUnderCompaction();
        }
        
        // Computes a hash value for a given string key to be used as an index in a hash table of size tableSize.
        inline int getHash(string key, int tableSize){
            int hash_val = 0;
            long long exp = 1;
            int prime = 31;
            for(auto &ch: key){
                hash_val = (hash_val + (exp * ch)) % tableSize;
                exp = (exp*prime) % tableSize;
            }
            return hash_val;
        }

        inline float getLoadFactor(){
            return keysPresent/primaryDbSize;
        }

        void checkForRehash(){
            // do not perform checks for rehash if already under rehash or compaction for stability and loss prevention
            if(underRehash || underCompaction()) {
                return;
            }
            float loadFactor = getLoadFactor();
            if(loadFactor >= loadFactorToScaleUp){
                if(primaryDbSize < maxTableSize){
                    rehashDbSize = (primaryDbSize << 1)+1;
                    primaryDbIdxForRehash = 0;
                    underRehash = true;
                    rehashDb = vector<vector<Object*>> (rehashDbSize);
                }
            }
            else if(loadFactor <= loadFactorToScaleDown){
                if(primaryDbSize > minTableSize){
                    rehashDbSize = (primaryDbSize >> 1)+1;
                    primaryDbIdxForRehash = 0;
                    underRehash = true;
                    rehashDb = vector<vector<Object*>> (rehashDbSize);
                }
            }
        }
        /*
            Rehashes the db to stabilize the load factor. This is done via incremental batch rehashing
            Also performs expiration of keys simultaneously as the expiry loop is blocked during the process.
        */
        void Rehash(){
            if(primaryDbIdxForRehash == primaryDbSize){
                // do not complete rehash if under compaction to prevent loss of keys
                if(underCompaction()) {
                    return;
                }
                underRehash = false;
                primaryDb = rehashDb;
                primaryDbSize = rehashDbSize;
                rehashDb = vector<vector<Object*>> ();
                return;
            }
            int newHash;
            int curBatchSize = 0;
            int rehashIdx = primaryDbIdxForRehash;
            while(rehashIdx < primaryDbSize && curBatchSize < rehashBatchSize){
                if(primaryDb[rehashIdx].size() > 0){
                    for(int i = (int)primaryDb[rehashIdx].size()-1; i >= 0; i--){
                        if(primaryDb[rehashIdx][i]){
                            if(!primaryDb[rehashIdx][i]->expired()){
                                newHash = getHash(primaryDb[rehashIdx][i]->getKey(), rehashDbSize);
                                rehashDb[newHash].push_back(primaryDb[rehashIdx][i]);
                            }
                            else{
                                delete primaryDb[rehashIdx][i];
                                keysPresent--;
                            }
                        }
                    }
                    primaryDb[rehashIdx] = vector<Object*> ();
                }
                curBatchSize++;
                rehashIdx=primaryDbIdxForRehash + curBatchSize;
            }
            primaryDbIdxForRehash = rehashIdx;
        }

    public:
        // Clears the expired keys to free memory for new keys. This does incremental batch checks to perform expiration.
        void runExpiryLoop(){
            if(underRehash){
                primaryDbIdxForExpiry = 0;
                return;
            }
            if(primaryDbIdxForExpiry >= primaryDbSize){
                primaryDbIdxForExpiry = 0;
            }
            int curBatchSize = 0;
            int expireIdx = primaryDbIdxForExpiry;
            while(expireIdx < primaryDbSize && curBatchSize < expiryBatchSize){
                for(int i = (int)primaryDb[expireIdx].size()-1; i >= 0; i--){
                    if(!primaryDb[expireIdx][i]){
                        primaryDb[expireIdx][i] = primaryDb[expireIdx].back();
                        primaryDb[expireIdx].pop_back();
                    }
                    else if(primaryDb[expireIdx][i]->expired()){
                        delete primaryDb[expireIdx][i];
                        primaryDb[expireIdx][i] = primaryDb[expireIdx].back();
                        primaryDb[expireIdx].pop_back();

                        keysPresent--;
                    }
                }
                curBatchSize++;
                expireIdx=primaryDbIdxForExpiry + curBatchSize;
            }
            primaryDbIdxForExpiry = expireIdx;
        }
    
    private:
        void WALCompaction() {
            if(!underCompaction()){
                return;
            }
            if(compactionRowPrimary == primaryDb.size() && compactionRowRehash == rehashDb.size()) {
                wal->startCompactionLogDump();
                compactionRowPrimary = compactionRowRehash = 0;
                return;
            }
            int batchesLeft = compactionBatchSize;
            vector<string> commands;
            while(batchesLeft && (compactionRowPrimary < primaryDb.size() || compactionRowRehash < rehashDb.size())) {
                if(compactionRowPrimary < primaryDb.size()) {
                    for(auto &obj : primaryDb[compactionRowPrimary]) {
                        commands = obj->buildCommands();
                        for(auto &command: commands) {
                            wal->appendCompactionLog(command);
                        }
                    }
                    compactionRowPrimary++;
                } else {
                    for(auto &obj : rehashDb[compactionRowRehash]) {
                        commands = obj->buildCommands();
                        for(auto &command: commands) {
                            wal->appendCompactionLog(command);
                        }
                    }
                    compactionRowRehash++;
                }
                batchesLeft--;
            }
        }

    private:
        bool keyExistsHelper(string key, vector<vector<Object*>> &db, int dbSize){
            int hash = getHash(key, dbSize);
            for(int i = (int)db[hash].size()-1; i >= 0; i--){
                if(!db[hash][i]){
                    primaryDb[hash][i] = primaryDb[hash].back();
                    primaryDb[hash].pop_back();
                }
                else if(db[hash][i]->getKey() == key){
                    if(db[hash][i]->expired()){
                        delete db[hash][i];
                        primaryDb[hash][i] = primaryDb[hash].back();
                        primaryDb[hash].pop_back();
                        
                        keysPresent--;
                        return false;
                    }
                    return true;
                }
            }
            return false;
        }

        bool deleteKeyHelper(string key, vector<vector<Object*>> &db, int dbSize){
            int hash = getHash(key, dbSize);
            for(int i = 0; i < db[hash].size(); i++){
                if(!db[hash][i]){
                    primaryDb[hash][i] = primaryDb[hash].back();
                    primaryDb[hash].pop_back();
                }
                else if(db[hash][i]->getKey() == key){
                    delete db[hash][i];
                    primaryDb[hash][i] = primaryDb[hash].back();
                    primaryDb[hash].pop_back();
                    return true;
                }
            }
            return false;
        }
        
        char getTypeHelper(string key, vector<vector<Object*>> &db, int dbSize){
            int hash = getHash(key, dbSize);
            for(auto &obj: db[hash]){
                if (obj->getKey() == key){
                    return obj->getType();
                }
            }
            return ' ';
        }
        
        Object* getObjectHelper(string key, vector<vector<Object*>> &db, int dbSize){
            int hash = getHash(key, dbSize);
            for(int i = (int)db[hash].size()-1; i >= 0; i--){
                if(!db[hash][i]){
                    primaryDb[hash][i] = primaryDb[hash].back();
                    primaryDb[hash].pop_back();
                }
                else if(db[hash][i]->getKey() == key){
                    if(db[hash][i]->expired()){
                        delete db[hash][i];
                        primaryDb[hash][i] = primaryDb[hash].back();
                        primaryDb[hash].pop_back();

                        keysPresent--;
                        return NULL;
                    }
                    return db[hash][i];
                }
            }
            return NULL;
        }

        inline void insertObjectHelper(Object* obj, vector<vector<Object*>> &db, int dbSize){
            int hash = getHash(obj->getKey(), dbSize);
            db[hash].push_back(obj);
        }

    public:
        Database(){
            keysPresent = 0;
            primaryDbSize = minTableSize;
            underRehash = false;
            primaryDbIdxForExpiry = 0;
            primaryDb = vector<vector<Object*>> (primaryDbSize);

            wal = NULL;
            walActive = false;
            compactionRowPrimary = 0;
            compactionRowRehash = 0;
        }

        inline bool keyExists(string key){
            bool exists = keyExistsHelper(key, primaryDb, primaryDbSize);
            if(!exists && underRehash){
                exists = keyExistsHelper(key, rehashDb, rehashDbSize);
            }
            return exists;
        }
        inline void assertKeyExists(string key){
            if(!keyExists(key)){
                throw ERR_KEY_NOT_FOUND;
            }
        }

        void deleteKey(string key){
            WALCompaction();
            assertKeyExists(key);
            bool deleted = deleteKeyHelper(key, primaryDb, primaryDbSize);
            if(underRehash){
                if(!deleted){
                    deleteKeyHelper(key, rehashDb, rehashDbSize);
                }
                Rehash();
            }

            keysPresent--;

            if(!underRehash){
                checkForRehash();
            }
        }

        string getType(string key){
            WALCompaction();
            assertKeyExists(key);
            char _type = getTypeHelper(key, primaryDb, primaryDbSize);
            if(underRehash){
                if(_type == ' '){
                    _type = getTypeHelper(key, rehashDb, rehashDbSize);
                }
                Rehash();
            }
            string type = "";
            switch(_type){
                case STRING_DATATYPE:
                    type = "String";
                    break;
                case LIST_DATATYPE:
                    type = "List";
                    break;
                case HASH_DATATYPE:
                    type = "Hash";
                    break;
            }
            return type;
        }

        Object* getObject(string key){
            WALCompaction();
            Object* obj = getObjectHelper(key, primaryDb, primaryDbSize);
            if(underRehash){
                if(!obj){
                    obj = getObjectHelper(key, rehashDb, rehashDbSize);
                }
                Rehash();
            }
            return obj;
        }
        void insertObject(Object* obj){
            WALCompaction();
            if(!underRehash){
                insertObjectHelper(obj, primaryDb, primaryDbSize);
            }
            else{
                insertObjectHelper(obj, rehashDb, rehashDbSize);
                Rehash();
            }

            keysPresent++;

            if(!underRehash){
                checkForRehash();
            }
        }

        void addWAL(WAL *wal) {
            this->wal = wal;
            walActive = true;
        }

        void printDetails(){
            cout<<"keys: "<<keysPresent<<endl;
            cout<<"db size: "<<primaryDbSize<<endl;
            cout<<"Load Factor: "<<getLoadFactor()<<endl;
        }
};

#endif // Database_class