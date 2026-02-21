#include<vector>
#include<string>
#include<ctime>
#include<iostream>

#include "../models/models.h"

#ifndef Database_class
#define Database_class

using std::string;
using std::cout;
using std::endl;

class Database{
    private:
        const int minTableSize = 1 << 2;
        const int maxTableSize = 1 << 20;
        const float loadFactorToScaleUp = 1;
        const float loadFactorToScaleDown = 0.25;
        const int rehashBatchSize = 10;
        const int expiryBatchSize = 10;

        vector<vector<Object*>> primaryDb;
        vector<vector<Object*>> rehashDb;

    private:
        int primaryDbIdxForExpiry;

        float keysPresent;
        int primaryDbSize;

        int primaryDbIdxForRehash;
        int rehashDbSize;
        bool underRehash;
        
        int getHash(string key, int tableSize){
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
            float loadFactor = getLoadFactor();
            if(loadFactor >= loadFactorToScaleUp){
                if(primaryDbSize < maxTableSize){
                    rehashDbSize = primaryDbSize << 1;
                    primaryDbIdxForRehash = 0;
                    underRehash = true;
                    rehashDb = vector<vector<Object*>> (rehashDbSize);
                }
            }
            else if(loadFactor <= loadFactorToScaleDown){
                if(primaryDbSize > minTableSize){
                    rehashDbSize = primaryDbSize >> 1;
                    primaryDbIdxForRehash = 0;
                    underRehash = true;
                    rehashDb = vector<vector<Object*>> (rehashDbSize);
                }
            }
        }
        void Rehash(){
            if(primaryDbIdxForRehash == primaryDbSize){
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
                for(int i = primaryDb[rehashIdx].size()-1; i >= 0; i--){
                    if(!primaryDb[rehashIdx][i]->expired()){
                        newHash = getHash(primaryDb[rehashIdx][i]->getKey(), rehashDbSize);
                        rehashDb[newHash].push_back(primaryDb[rehashIdx][i]);
                    }
                    else{
                        delete primaryDb[rehashDbSize][i];
                        keysPresent--;
                    }
                    primaryDb[rehashIdx].pop_back();
                }
                curBatchSize++;
                rehashIdx=primaryDbIdxForRehash + curBatchSize;
            }
            primaryDbIdxForRehash = rehashIdx;
        }

    public:
        void runExpiryLoop(){
            if(underRehash){
                return;
            }
            if(primaryDbIdxForExpiry >= primaryDbSize){
                primaryDbIdxForExpiry = 0;
            }
            int curBatchSize = 0;
            int expireIdx = primaryDbIdxForExpiry;
            while(expireIdx < primaryDbSize && curBatchSize < expiryBatchSize){
                for(int i = primaryDb[expireIdx].size()-1; i >= 0; i--){
                    if(primaryDb[expireIdx][i]->expired()){
                        delete primaryDb[expireIdx][i];
                        primaryDb[expireIdx].erase(primaryDb[expireIdx].begin() + i);

                        keysPresent--;
                    }
                }
                curBatchSize++;
                expireIdx=primaryDbIdxForExpiry + curBatchSize;
            }
            primaryDbIdxForExpiry = expireIdx;
        }
        

    private:
        bool keyExistsHelper(string key, vector<vector<Object*>> &db, int dbSize){
            int hash = getHash(key, dbSize);
            for(int i = db[hash].size()-1; i >= 0; i--){
                if(db[hash][i]->getKey() == key){
                    if(db[hash][i]->expired()){
                        delete db[hash][i];
                        db[hash].erase(db[hash].begin()+i);

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
                if(db[hash][i]->getKey() == key){
                    delete db[hash][i];
                    db[hash].erase(db[hash].begin() + i);
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
            for(int i = db[hash].size()-1; i >= 0; i--){
                if(db[hash][i]->getKey() == key){
                    if(db[hash][i]->expired()){
                        delete db[hash][i];
                        db[hash].erase(db[hash].begin()+i);

                        keysPresent--;
                        return NULL;
                    }
                    return db[hash][i];
                }
            }
            return NULL;
        }

        void insertObjectHelper(Object* obj, vector<vector<Object*>> &db, int dbSize){
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
        }

        bool keyExists(string key){
            bool exists = keyExistsHelper(key, primaryDb, primaryDbSize);
            if(!exists && underRehash){
                exists = keyExistsHelper(key, rehashDb, rehashDbSize);
            }
            return exists;
        }
        void assertKeyExists(string key){
            if(!keyExists(key)){
                throw string("Error: Key not found");
            }
        }

        void deleteKey(string key){
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
                case 's':
                    type = "String";
                    break;
                case 'l':
                    type = "List";
                    break;
            }
            return type;
        }

        Object* getObject(string key){
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

        void printLoadFactor(){
            cout<<"Load Factor: "<<getLoadFactor()<<endl;
        }
};

#endif // Database_class