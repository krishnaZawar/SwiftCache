#include<iostream>
#include "parser.cpp"
#include "database/database.cpp"
#include<winsock2.h>
#include<ws2tcpip.h>
#include<windows.h>

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
    
    char buffer[4096];
    string command;
    string resp;
    while(true){
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0'; // Null-terminate
        }else if (bytesReceived == 0) {
            cout << "Client disconnected" << endl;
            break;
        } else {
            cerr << "recv() error: " << WSAGetLastError() << endl;
            break;
        }
        command = string (buffer, bytesReceived);
        if(command == "EXIT"){
            break;
        }
        try {
            resp = parser.parseCommand(command);
        }
        catch(string &err){
            cout << err << endl;
            resp = err;
        }
        catch(exception &e){
            cout << e.what() << endl;
            resp = e.what();
        }
        send(clientSocket, resp.c_str(), resp.size(), 0);
    }

    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    db.printLoadFactor();

    return 0;
}

// using std::time;

// int main(){
//     string command;

//     Database db = Database();
//     Parser parser = Parser(&db);
    
//     long long start = time(NULL);
//     for(int i = 0; i < 10000000; i++){
//         command = "SET k"+to_string(i)+" v"+to_string(i);
//         try{
//             parser.parseCommand(command);
//         }catch(...) {}
//     }
//     long long end = time(NULL);
//     cout << end-start << endl;
//     while(true){
//         cout<<">> ";
//         getline(cin, command);
//         if(command == "EXIT"){
//             break;
//         }
//         try{  
//             cout<<parser.parseCommand(command)<<endl;
//             db.printLoadFactor();
//         }
//         catch(string &err){
//             cout<<err<<endl;
//         }
//         catch(exception &e){
//             cout<<e.what()<<endl;
//         }
//     }

//     db.printLoadFactor();

//     return 0;
// }