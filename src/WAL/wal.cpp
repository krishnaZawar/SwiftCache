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
        vector<string> buffer;

        atomic<bool> isDumping;
        int dumpInterval;
        string dumpFile;
        thread dumpWorker;

    public:
        WAL(string dumpFile, int dumpInterval) {
            this->dumpFile = dumpFile;
            this->dumpInterval = dumpInterval;

            isRunning = true;
            dumpWorker = thread(&WAL::Run, this);
        }

        void appendLog(string log) {
            mu.lock();
            if(isDumping){
                buffer.push_back(log);
            } else {
                logs.push_back(log);
            }
            mu.unlock();
        }

        void dumpLogs() {
            /*
                Dump algorithm:
                    1. Dump the buffer logs first
                    2. Go to dump state
                    3. Dump the main logs
                    4. Come out of dump state
            */

            ofstream file (dumpFile, std::ios::app);
            if(!file) {
                throw string("Error: could not open WAL file");
            }

            // dump buffer
            for (auto &log : buffer) {
                file << log << "\n";
            }
            buffer = {};

            // go to dump state
            isDumping = true;

            // dump main logs
            for (auto &log : logs) {
                file << log << "\n";
            }
            logs = {};

            // come out of dump state
            isDumping = false;

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