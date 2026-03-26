#pragma once

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>

using Socket = SOCKET;
constexpr Socket INVALID_SOCKET_FD = INVALID_SOCKET;
constexpr int SOCKET_ERROR_CODE = SOCKET_ERROR;
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

using Socket = int;
constexpr Socket INVALID_SOCKET_FD = -1;
constexpr int SOCKET_ERROR_CODE = -1;
#endif

bool initSockets();
void cleanupSockets();
int getSocketError();
int closeSocket(Socket socketFd);
