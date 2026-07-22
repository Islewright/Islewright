#ifndef ISLEWRIGHT_CLIENTINFO_HPP
#define ISLEWRIGHT_CLIENTINFO_HPP

#include <WinSock2.h>
#include <cstring>

namespace islewright::clientinfo {

struct ClientInfo
{
    SOCKET m_socket = INVALID_SOCKET;
    SOCKADDR_IN m_clientAddr{};

    static constexpr int BUFFER_SIZE = 1024;

    char* m_recvBuffer = nullptr;
    char* m_sendBuffer = nullptr;

    ClientInfo(SOCKET clientSocket, const SOCKADDR_IN& clientAddr)
        : m_socket(clientSocket), m_clientAddr(clientAddr)
    {
        m_recvBuffer = new char[BUFFER_SIZE + 1];
        std::memset(m_recvBuffer, 0, BUFFER_SIZE + 1);
        m_sendBuffer = new char[BUFFER_SIZE + 1];
        std::memset(m_sendBuffer, 0, BUFFER_SIZE + 1);
    }

    ~ClientInfo()
    {
        delete[] m_recvBuffer;
        delete[] m_sendBuffer;
    }
};

} // namespace islewright::clientinfo

#endif // ISLEWRIGHT_CLIENTINFO_HPP