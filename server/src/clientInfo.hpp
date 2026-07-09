#ifndef CLIENTINFO_H
#define CLIENTINFO_H

#include <WinSock2.h>

#define MAX_BUF_SIZE 1024

struct stClientInfo 
{
    SOCKET socket_;
    UINT64 clientIndex_;
    char recvbuf_[MAX_BUF_SIZE];    
};

#endif