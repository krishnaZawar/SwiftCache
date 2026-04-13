#include<string>
#include<vector>
#include<mutex>
#include<fstream>
#include<atomic>
#include<thread>
#include<chrono>
#if __has_include(<filesystem>) && __cplusplus >= 201703L
#include<filesystem>
#define OS_WINDOWS 1
#else
#define OS_WINDOWS 0
#endif
#include<cstdio>
#include<condition_variable>
#include<iostream>

#ifndef WAL_class
#define WAL_class

using std::string;
using std::vector;
using std::move;
using std::mutex;
using std::unique_lock;
using std::condition_variable;
using std::ofstream;
using std::atomic;
using std::thread;
using std::this_thread::sleep_for;
using std::chrono::seconds;
using std::cout;
using std::endl;

class WAL {
    private:
        atomic<bool> isRunning;
        atomic<bool> underCompaction;
        atomic<bool> startCompactionDumpFlag;

        mutex mu;
        vector<string> logs;

        condition_variable cv;
        mutex compactionMu;
        vector<string> compactionLogs;

        int dumpInterval;
        string dumpFile;
        thread dumpWorker;

        int compactionInterval;
        string compactionDumpFile;
        thread compactionLogDumpWorker;

        void dumpLogs() {
            /*
                Dump algorithm:
                    1. Dump local logs
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
            localLogs.clear();

            file.close();
        }
        void Run() {
            while(isRunning) {
                sleep_for(seconds(dumpInterval));
                dumpLogs();
            }
        }

        void dumpCompactionLogs() {
            /*
                Dump algorithm:
                    1. Dump compaction logs
                    2. atomically rename file
                    3. delete temp file
            */

            ofstream file (compactionDumpFile, std::ios::app);
            if(!file) {
                throw string("Error: could not open WAL file");
            }

            vector<string> localLogs;

            compactionMu.lock();
            localLogs = move(compactionLogs);
            compactionMu.unlock();

            for(auto &log : localLogs) {
                file << log << "\n";
            }
            localLogs.clear();

            file.close();
            
            try {
#if OS_WINDOWS
                std::filesystem::rename(compactionDumpFile, dumpFile);
                std::filesystem::remove(compactionDumpFile);
#else
                std::rename(compactionDumpFile.c_str(), dumpFile.c_str());
                std::remove(compactionDumpFile.c_str());
#endif
            } catch(...) {
                std::remove(compactionDumpFile.c_str());
            }
        }
        void RunCompactionThread() {
            while(isRunning) {
                sleep_for(seconds(compactionInterval));
                std::cout << "start compaction" << std::endl;
                underCompaction = true;
                unique_lock<mutex> lock(mu);
                cv.wait(lock, [&]{ return startCompactionDumpFlag.load() || !isRunning.load(); });
                if (!isRunning) break;
                
                underCompaction = false;

                lock.unlock();  // release lock while dumping
                
                std::cout << "start dump" << std::endl;
                dumpCompactionLogs();

                startCompactionDumpFlag = false;
            }
        }



    public:
        WAL(string dumpFile, int dumpInterval, int compactionInterval) {
            this->dumpFile = dumpFile;
            this->dumpInterval = dumpInterval;
            this->compactionInterval = compactionInterval;

            compactionDumpFile = dumpFile.substr(0, dumpFile.size()-4) + "_compact.txt";
            
            underCompaction = false;
            startCompactionDumpFlag = false;

            isRunning = true;
            dumpWorker = thread(&WAL::Run, this);
            compactionLogDumpWorker = thread(&WAL::RunCompactionThread, this);
        }

        inline void appendLog(string log) {
            mu.lock();
            logs.push_back(log);
            mu.unlock();

            if(underCompaction) {
                appendCompactionLog(log);
            }
        }

        inline void startCompactionLogDump() {
            startCompactionDumpFlag = true;
            cv.notify_one();    // notify the wait() for completion
        }

        inline bool isUnderCompaction() {
            return underCompaction;
        }

        inline void appendCompactionLog(string log) {
            compactionMu.lock();
            compactionLogs.push_back(log);
            compactionMu.unlock();
        }

        ~WAL() {
            // stop running and notify_all threads
            isRunning = false;
            cv.notify_all();
            if(dumpWorker.joinable()){
                dumpWorker.join();
                std::cout << "closed main dump worker" << std::endl;
            }
            // force stop compaction for server shutdown
            underCompaction = false;
            if(compactionLogDumpWorker.joinable()){
                compactionLogDumpWorker.join();
                std::cout << "closed compaction dump worker" << std::endl;
            }
        }
};

#endif // WAL_class