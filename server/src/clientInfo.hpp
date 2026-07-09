#ifndef CLIENTINFO_H
#define CLIENTINFO_H

#include <WinSock2.h>
#include <thread>

#define MAX_BUF_SIZE 1024

struct stClientInfo 
{
    SOCKET socket_ = INVALID_SOCKET;
    UINT64 clientIndex_ = -1;
    std::thread recvThread_;
};

#endif