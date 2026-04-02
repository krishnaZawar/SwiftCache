#include<iostream>
#include<string>
#include<string>
#include<thread>
#include<fstream>

#include "parser.cpp"
#include "WAL/wal.cpp"
#include "database/database.cpp"
#include "platform/socket.cpp"
#include "utils/utils.h"

// #include<ctime>

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#endif

using std::exception;
using std::cout;
using std::getline;
using std::cin;
using std::endl;
using std::cerr;
using std::stoi;
using std::string;
using std::ifstream;

int main(int argc, char* argv[]){
    int port = 8080;
    string dumpFile = "";
    int dumpInterval = 0;
    // compaction interval is by default 5 * dumpInterval if not specified
    int compactionInterval = 0;

    if(argc % 2 == 0) {
        cerr << "Error: Invalid number of CLI arguments passed" << endl;
        return 1;
    }

    for(int i = 1; i < argc; i += 2) {
        if(strcmp(argv[i], "-p") == 0) {
            try {
                port = strToInt(argv[i+1]);
                if(port <= 0) {
                    throw string("positive integer expected");
                }
            } catch(...) {
                cerr << "Error: Port number should be an integer" << endl;
                return 1;
            }
        } else if(strcmp(argv[i], "-df") == 0) {
            dumpFile = argv[i+1];
            if(dumpFile.size() < 4 || !endsWith(dumpFile, ".txt")) {
                cerr << "Error: Dump file should be a .txt file" << endl;
                return 1;
            }
        } else if(strcmp(argv[i], "-di") == 0) {
            try {
                dumpInterval = strToInt(argv[i+1]);
                if(dumpInterval <= 0) {
                    throw string("positive integer expected");
                }
            } catch(...) {
                cerr << "Error: Dump interval should be an integer and non-negative" << endl;
                return 1;
            }
        } else if(strcmp(argv[i], "-ci") == 0) {
            try {
                compactionInterval = strToInt(argv[i+1]);
                if(compactionInterval <= 0) {
                    throw string("positive integer expected");
                }
            } catch(...) {
                cerr << "Error: Compaction interval should be an integer and non-negative" << endl;
                return 1;
            }
        } else {
            cerr << "Error: Invalid flag passed" << endl;
            return 1;
        }
    }

    if(dumpInterval != 0 && dumpFile == "") {
        cerr << "Error: Dump File not specified" << endl;
        return 1;
    }
    if(dumpFile != "" && dumpInterval == 0) {
        cerr << "Error: Specify dump interval" << endl;
        return 1;
    }

    if (!initSockets()) {
        cerr << "Socket initialization failed: " << getSocketError() << endl;
        return 1;
    }

    Socket serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET_FD) {
        cerr << "Failed to create socket: " << getSocketError() << endl;
        cleanupSockets();
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port); // port number
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR_CODE) {
        cerr << "Bind failed: " << getSocketError() << endl;
        closeSocket(serverSocket);
        cleanupSockets();
        return 1;
    }
    
    WAL *wal = NULL;
    Database db;
    Parser parser (&db);

    u_int64 logCount = 0;
    
    if (dumpFile != "") {
        // load WAL into DB
        ifstream file (dumpFile);
        if(file) {
            string walLog;
            while(getline(file, walLog)){
                try{
                    parser.parseCommand(walLog.c_str(), walLog.size(), true);
                    logCount++;
                } catch(...) {}
            }
        }
        if(compactionInterval == 0) {
            compactionInterval = 5 * dumpInterval;
        }
        wal = new WAL(dumpFile, dumpInterval, compactionInterval);
        parser.addWAL(wal);
        db.addWAL(wal);
    }
    
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR_CODE) {
        cerr << "Listen failed: " << getSocketError() << endl;
        closeSocket(serverSocket);
        cleanupSockets();
        return 1;
    }
    
    cout << "Server is listening on port " << port << "..." << endl;
    
    Socket clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket == INVALID_SOCKET_FD) {
        cerr << "Accept failed: " << getSocketError() << endl;
        closeSocket(serverSocket);
        cleanupSockets();
        return 1;
    }
    
    cout << "Client connected!" << endl;
    
    char command[4096];
    string resp;
    while(true){
        int bytesReceived = recv(clientSocket, command, sizeof(command) - 1, 0);
        if (bytesReceived > 0) {
            command[bytesReceived] = '\0'; // Null-terminate
        }else if (bytesReceived == 0) {
            cout << "Client disconnected" << endl;
            break;
        } else {
            cerr << "recv() error: " << getSocketError() << endl;
            break;
        }
        if(!strcmp(command, "EXIT")){
            break;
        }
        try {
            resp = parser.parseCommand(command, bytesReceived);
        }
        catch(string &err){
            resp = err;
        }
        catch(exception &e){
            resp = e.what();
        }
        send(clientSocket, resp.c_str(), static_cast<int>(resp.size()), 0);
    }

    closeSocket(clientSocket);
    closeSocket(serverSocket);
    cleanupSockets();

    db.printDetails();

    if(wal) {
        delete wal;
    }

    return 0;
}
