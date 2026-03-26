#include<iostream>
#include<string>
#include<cstring>

#include "parser.cpp"
#include "database/database.cpp"
#include "platform/socket.cpp"

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

int main(int argc, char* argv[]){
    int PORT;
    if(argc == 1){
        PORT = 8080;
    }
    else if(argc == 3 && string(argv[1]) == "-p"){
        PORT = stoi(string(argv[2]));
    }
    else{
        cerr << "Error: Invalid initialization of DB" << endl;
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
    serverAddr.sin_port = htons(PORT); // Port number
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR_CODE) {
        cerr << "Bind failed: " << getSocketError() << endl;
        closeSocket(serverSocket);
        cleanupSockets();
        return 1;
    }

    Database db;
    Parser parser (&db);

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR_CODE) {
        cerr << "Listen failed: " << getSocketError() << endl;
        closeSocket(serverSocket);
        cleanupSockets();
        return 1;
    }

    cout << "Server is listening on port " << PORT << "..." << endl;

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

    return 0;
}
