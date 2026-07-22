#ifndef ISLEWRIGHT_CLIENTINFO_HPP
#define ISLEWRIGHT_CLIENTINFO_HPP

#include <WinSock2.h>

namespace islewright::clientinfo {

struct ClientInfo
{
    SOCKET m_socket = INVALID_SOCKET;
    SOCKADDR_IN m_clientAddr{};

    static constexpr int m_bufferSize = 1024;

    char* m_recvBuffer = nullptr;
    char* m_sendBuffer = nullptr;

    ClientInfo(SOCKET clientSocket, const SOCKADDR_IN& clientAddr)
        : m_socket(clientSocket), m_clientAddr(clientAddr)
    {
        m_recvBuffer = new char[m_bufferSize + 1];
        memset(m_recvBuffer, 0, m_bufferSize + 1);
        m_sendBuffer = new char[m_bufferSize + 1];
        memset(m_sendBuffer, 0, m_bufferSize + 1);
    }

    ~ClientInfo()
    {
        delete[] m_recvBuffer;
        delete[] m_sendBuffer;
    }
};

} // namespace islewright::clientinfo

#endif // ISLEWRIGHT_CLIENTINFO_HPP