#include<iostream>
#include<string>
#include<winsock2.h>
#include<ws2tcpip.h>
#include<windows.h>

#include "parser.cpp"
#include "database/database.cpp"

// #include<ctime>

#pragma comment(lib, "ws2_32.lib") // Link Winsock library

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

    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData); // Request Winsock 2.2
    if (result != 0) {
        cerr << "WSAStartup failed: " << result << endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Failed to create socket: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT); // Port number
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Bind failed: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    Database db;
    Parser parser (&db);

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        cerr << "Listen failed: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "Server is listening on port " << PORT << "..." << endl;

    SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Accept failed: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
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
            cerr << "recv() error: " << WSAGetLastError() << endl;
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
        send(clientSocket, resp.c_str(), resp.size(), 0);
    }

    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    db.printDetails();

    return 0;
}
