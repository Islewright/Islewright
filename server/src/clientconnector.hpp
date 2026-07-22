#ifndef ISLEWRIGHT_CLIENTCONNECTOR_HPP
#define ISLEWRIGHT_CLIENTCONNECTOR_HPP

#include "clientinfo.hpp"

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <cstring>
#include <iostream>
#include <string>
#include <thread>

namespace islewright::clientconnector {

using ClientInfo = islewright::clientinfo::ClientInfo;

class ClientConnector
{
  public:
    ClientConnector(const USHORT port = 9000) : m_port(port)
    {
        WSADATA wsaData;
        int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);

        if (ret != 0)
        {
            throw std::runtime_error("[ERROR] WSAStartup failed with error: " + std::to_string(ret));
        }
    }

    ~ClientConnector()
    {
        EndNetworking();
        CloseListenSocket();

        delete m_clientInfo;
        m_clientInfo = nullptr;

        WSACleanup();

        OnDisConnect();
    }

    // Getters
    USHORT GetPort()
    {
        return m_port;
    }

    // Setters
    void SetPort(USHORT port)
    {
        m_port = port;
    }

    // Create server socket and start listening
    bool Listen()
    {
        m_listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if (m_listenSocket == INVALID_SOCKET)
        {
            std::cout << "[ERROR] socket() failed:" << WSAGetLastError() << "\n";
            return false;
        }

        SOCKADDR_IN serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        serverAddr.sin_port = htons(m_port);

        int ret = bind(m_listenSocket, reinterpret_cast<SOCKADDR*>(&serverAddr),
                       sizeof(serverAddr));

        if (ret == SOCKET_ERROR)
        {
            std::cout << "[ERROR] bind() failed:" << WSAGetLastError() << "\n";
            CloseListenSocket();
            return false;
        }

        ret = listen(m_listenSocket, 1);

        if (ret == SOCKET_ERROR)
        {
            std::cout << "[ERROR] listen() failed:" << WSAGetLastError() << "\n";
            CloseListenSocket();
            return false;
        }

        return true;
    }

    // Wait for one client connection
    bool Accept()
    {
        if (m_listenSocket == INVALID_SOCKET || m_clientInfo != nullptr)
        {
            return false;
        }

        SOCKADDR_IN clientAddr{};
        int clientAddrSize = sizeof(clientAddr);

        SOCKET clientSocket =
            accept(m_listenSocket, reinterpret_cast<SOCKADDR*>(&clientAddr), &clientAddrSize);

        if (clientSocket == INVALID_SOCKET)
        {
            std::cout << "[ERROR] accept() failed:" << WSAGetLastError() << "\n";
            return false;
        }

        m_clientInfo = new ClientInfo(clientSocket, clientAddr);

        OnConnect();

        return true;
    }

    // Start receive and send data from client
    void StartNetworking()
    {
        if (m_clientInfo == nullptr)
        {
            return;
        }

        m_isNetworking = true;
        CreateRecvThread();
    }

    void EndNetworking()
    {
        m_isNetworking = false;

        if (m_clientInfo != nullptr &&
            m_clientInfo->m_socket != INVALID_SOCKET)
        {
            shutdown(m_clientInfo->m_socket, SD_BOTH);
            CloseClientSocket();
        }

        if (m_recvThread.joinable())
        {
            m_recvThread.join();
        }
    }

    bool Send(const char* msg, const int len)
    {
        if (msg == nullptr || len <= 0 || m_clientInfo == nullptr ||
            len > ClientInfo::m_bufferSize || m_isNetworking == false)
        {
            return false;
        }

        std::memcpy(m_clientInfo->m_sendBuffer, msg, len);

        int ret = send(m_clientInfo->m_socket, m_clientInfo->m_sendBuffer, len, 0);

        if (ret == SOCKET_ERROR)
        {
            std::cout << "[ERROR] send() failed:" << WSAGetLastError() << "\n";
            return false;
        }

        return true;
    }

    virtual void OnConnect() {}

    virtual void OnReceive(char* message, int len) {}

    virtual void OnDisConnect() {}

  private:
    void CloseListenSocket()
    {
        if (m_listenSocket == INVALID_SOCKET)
        {
            return;
        }

        closesocket(m_listenSocket);
        m_listenSocket = INVALID_SOCKET;
    }

    void CloseClientSocket()
    {
        if (m_clientInfo == nullptr ||
            m_clientInfo->m_socket == INVALID_SOCKET)
        {
            return;
        }

        closesocket(m_clientInfo->m_socket);
        m_clientInfo->m_socket = INVALID_SOCKET;
    }

    void CreateRecvThread()
    {
        m_recvThread = std::thread([this]() { Recv(); });
    }

    void Recv()
    {
        while (m_isNetworking)
        {
            int ret = recv(m_clientInfo->m_socket, m_clientInfo->m_recvBuffer,
                           ClientInfo::m_bufferSize, 0);

            if (ret > 0)
            {
                OnReceive(m_clientInfo->m_recvBuffer, ret);
            }
            else if (ret == 0)
            {
                std::cout << "[RECV] Client connection closed\n";
                m_isNetworking = false;
            }
            else
            {
                if (m_isNetworking)
                {
                    std::cout << "[ERROR] recv() failed:" << WSAGetLastError() << "\n";
                }

                m_isNetworking = false;
            }
        }
    }

    SOCKET m_listenSocket = INVALID_SOCKET;
    USHORT m_port;

    ClientInfo* m_clientInfo = nullptr;

    bool m_isNetworking = false;

    std::thread m_recvThread;
};

} // namespace islewright::clientconnector

#endif // ISLEWRIGHT_CLIENTCONNECTOR_HPP