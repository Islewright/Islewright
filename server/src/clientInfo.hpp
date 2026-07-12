#ifndef CLIENTINFO_HPP
#define CLIENTINFO_HPP

#include <WinSock2.h>
#include <thread>

#define MAX_BUF_SIZE 1024

struct stClientInfo 
{
    SOCKET socket = INVALID_SOCKET;
    UINT64 clientIndex = -1;
    SOCKADDR_IN clientAddr;
    std::thread recvThread;
};

#endif