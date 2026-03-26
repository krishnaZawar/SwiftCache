#include "socket.h"

#ifndef _WIN32
#include <cerrno>
#endif

bool initSockets() {
#ifdef _WIN32
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
#else
    return true;
#endif
}

void cleanupSockets() {
#ifdef _WIN32
    WSACleanup();
#endif
}

int getSocketError() {
#ifdef _WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}

int closeSocket(Socket socketFd) {
#ifdef _WIN32
    return closesocket(socketFd);
#else
    return close(socketFd);
#endif
}
