#include<string>
#include<vector>
#include<mutex>
#include<fstream>
#include<atomic>
#include<thread>
#include<chrono>

#ifndef WAL_class
#define WAL_class

using std::string;
using std::vector;
using std::move;
using std::mutex;
using std::ofstream;
using std::atomic;
using std::thread;
using std::this_thread::sleep_for;
using std::chrono::seconds;

class WAL {
    private:
        atomic<bool> isRunning;

        mutex mu;
        vector<string> logs;

        uint64_t logCount;

        int dumpInterval;
        string dumpFile;
        thread dumpWorker;

    public:
        WAL(string dumpFile, int dumpInterval, int logCount) {
            this->dumpFile = dumpFile;
            this->dumpInterval = dumpInterval;
            this->logCount = logCount;
            
            isRunning = true;
            dumpWorker = thread(&WAL::Run, this);
        }

        void appendLog(string log) {
            mu.lock();
            logs.push_back(log);
            mu.unlock();
        }

        void dumpLogs() {
            /*
                Dump algorithm:
                    1. Swap the data from logs to localLogs
                    2. Free the logs
                    3. dump local logs
            */

            ofstream file (dumpFile, std::ios::app);
            if(!file) {
                throw string("Error: could not open WAL file");
            }

            vector<string> localLogs;

            mu.lock();
            localLogs = move(logs);
            mu.unlock();

            for(auto &log : localLogs) {
                file << log << "\n";
            }
            logCount += localLogs.size();
            localLogs.clear();

            file.close();
        }

        void Run() {
            while(isRunning) {
                sleep_for(seconds(dumpInterval));
                dumpLogs();
            }
        }

        ~WAL() {
            isRunning = false;
            if(dumpWorker.joinable()){
                dumpWorker.join();
            }
        }
};

#endif // WAL_class