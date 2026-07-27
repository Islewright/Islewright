#ifndef ISLEWRIGHT_CLIENTINFO_HPP
#define ISLEWRIGHT_CLIENTINFO_HPP

#include <WinSock2.h>
#include <cstring>

namespace islewright::clientinfo {

struct ClientInfo
{
    SOCKET socket = INVALID_SOCKET;
    SOCKADDR_IN clientAddr{};

    static constexpr int BUFFER_SIZE = 1024;

    char* recvBuffer = nullptr;
    char* sendBuffer = nullptr;

    ClientInfo(SOCKET clientSocket, const SOCKADDR_IN& clientAddr)
        : socket(clientSocket), clientAddr(clientAddr)
    {
        recvBuffer = new char[BUFFER_SIZE + 1];
        std::memset(recvBuffer, 0, BUFFER_SIZE + 1);
        sendBuffer = new char[BUFFER_SIZE + 1];
        std::memset(sendBuffer, 0, BUFFER_SIZE + 1);
    }

    ~ClientInfo()
    {
        delete[] recvBuffer;
        delete[] sendBuffer;
    }
};

} // namespace islewright::clientinfo

#endif // ISLEWRIGHT_CLIENTINFO_HPP