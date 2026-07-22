#ifndef ISLEWRIGHT_TCPCONNECTOR_HPP
#define ISLEWRIGHT_TCPCONNECTOR_HPP

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <iostream>
#include <string>
#include <thread>

namespace islewright::tcpconnector {

class TcpConnector
{
  public:
    TcpConnector(const std::string hostAddr = "127.0.0.1", const USHORT port = 9000) : m_serverAddr(hostAddr), m_port(port) 
    {
        WSADATA wsaData;
        int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (ret != 0) 
        {
            throw std::runtime_error("[ERROR] WSAStartup failed with error: " + std::to_string(ret));
        }

        m_recvBuffer = new char[m_bufferSize + 1];
        m_sendBuffer = new char[m_bufferSize + 1];
    }

    ~TcpConnector()
    {
        EndNetworking();

        delete[] m_recvBuffer;
        delete[] m_sendBuffer;

        WSACleanup();
        
        OnDisConnect();
    }

    // Getters
    std::string GetHost()
    {
        return m_serverAddr;
    }

    USHORT GetPort()
    {
        return m_port;
    }
    
    // Setters
    void SetHost(std::string hostAddr)
    {
        m_serverAddr = hostAddr;
    }

    void SetPort(USHORT port)
    {
        m_port = port;
    }

    // Connect to Server
    bool Connect()
    {
        m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (m_socket == INVALID_SOCKET) 
        {
            std::cout << "[ERROR] socket() failed:" << WSAGetLastError() << "\n";
            return false;
        }

        SOCKADDR_IN serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(m_port);
        inet_pton(AF_INET, m_serverAddr.c_str(), &serverAddr.sin_addr);

        int ret = connect(m_socket, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN));
        if (ret == SOCKET_ERROR) 
        {
            CloseSocket();
            return false;
        }

        OnConnect();

        return true;
    }

    // Start receive and send data from server
    void StartNetworking()
    {
        m_isNetworking = true;
        CreateRecvThread();
    }

    void EndNetworking()
    {
        m_isNetworking = false;

        if (m_socket != INVALID_SOCKET)
        {
            CloseSocket();
        }

        if (m_recvThread.joinable())
        {
            m_recvThread.join();
        }
    }

    bool Send(const char* msg, const int len)
    {
        if (msg == nullptr || len <= 0 || len > m_bufferSize || m_isNetworking == false) 
        {
            return false;
        }

        std::memcpy(m_sendBuffer, msg, len);

        int ret = send(m_socket, m_sendBuffer, len, 0);
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
    void CloseSocket()
    {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
    }

    void CreateRecvThread()
    {
        m_recvThread = std::thread([this]() { Recv(); });
    }

    // TODO: Create callback method of received data
    void Recv()
    {
        while (m_isNetworking)
        {
            int ret = recv(m_socket, m_recvBuffer, m_bufferSize, 0);
            
            if (ret > 0) 
            {
                OnReceive(m_recvBuffer, ret);
            }
            else if (ret == 0) 
            {
                std::cout << "[RECV] Server connection closed\n";
                m_isNetworking = false;
            }
            else 
            {
                std::cout << "[ERROR] recv() failed:" << WSAGetLastError() << "\n";
                m_isNetworking = false;
            }
        }
    }

    SOCKET m_socket = INVALID_SOCKET;
    std::string m_serverAddr;
    USHORT m_port;

    bool m_isNetworking = false;

    std::thread m_recvThread;

    static constexpr int m_bufferSize = 1024;
    char* m_recvBuffer = nullptr;
    char* m_sendBuffer = nullptr;
};

} // namespace islewright::tcpconnector

#endif // ISLEWRIGHT_TCPCONNECTOR_HPP