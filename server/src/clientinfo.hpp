#ifndef ISLEWRIGHT_CLIENTINFO_HPP
#define ISLEWRIGHT_CLIENTINFO_HPP

#include "islewright/common/networklimits.hpp"

#include <WinSock2.h>

namespace islewright::clientinfo {

struct ClientInfo
{
    SOCKET socket = INVALID_SOCKET;
    SOCKADDR_IN clientAddr{};

    char* recvBuffer = nullptr;

    ClientInfo(SOCKET clientSocket, const SOCKADDR_IN& clientAddr)
        : socket(clientSocket), clientAddr(clientAddr)
    {
        recvBuffer = new char[common::networklimits::RECEIVE_BUFFER_SIZE];
    }

    ~ClientInfo()
    {
        delete[] recvBuffer;
    }
};

} // namespace islewright::clientinfo

#endif // ISLEWRIGHT_CLIENTINFO_HPP
