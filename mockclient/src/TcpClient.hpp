#ifndef ISLEWRIGHT_TCP_CLIENT_HPP
#define ISLEWRIGHT_TCP_CLIENT_HPP

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <iostream>
#include <string>
#include <format>
#include <thread>


class TcpClient
{
 public:
    TcpClient() = default;

    ~TcpClient()
    {
        Stop();
        
        WSACleanup();
    }
    
    bool Init(UINT16 port)
    {
        WSAData wsa;

        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
            std::string msg = std::format("[ERROR] WSAStartup() failed: {0}\n", WSAGetLastError());
            std::cout << msg;

            return false;
        }

        m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if (m_socket == INVALID_SOCKET) {
            std::string msg = std::format("[ERROR] socket() failed: {0}\n", WSAGetLastError());
            std::cout << msg;

            return false;
        }
        
        m_port = port;
        return true;
    }

    bool Connect(const std::string serverIP = "127.0.0.1")
    {
        int ret;

        memset(&m_serverAddr, 0x00, sizeof(SOCKADDR_IN));
        m_serverAddr.sin_family = AF_INET;
        ret = InetPtonA(AF_INET, serverIP.c_str(), &m_serverAddr.sin_addr);

        if (ret != 1) {
            std::string msg = std::format("[ERROR] connect() failed: {0}\n", WSAGetLastError());
            return false;
        }

        m_serverAddr.sin_port = htons(m_port);
        ret = connect(m_socket, (struct sockaddr *)&m_serverAddr, sizeof(SOCKADDR_IN));

        if (ret == SOCKET_ERROR) {
            std::string msg = std::format("[ERROR] connect() failed: {0}\n", WSAGetLastError());
            std::cout << msg;

            closesocket(m_socket);
            m_socket = INVALID_SOCKET;
            return false;
        }

        std::cout << "[SYSTEM] Server connection success\n";

        m_isRunning = true;
        m_recvThread = std::thread([this]() {ReceiveLoop();});
        return true;
    }

    bool SendMessage(const std::string& message)
    {
        if (m_socket == INVALID_SOCKET || !m_isRunning) {
            return false;
        }

        int total_sent = 0;
        int msg_len = static_cast<int>(message.length());

        while (total_sent < msg_len) {
            int sent = send(m_socket, message.c_str() + total_sent, msg_len - total_sent, 0);

            if (sent == SOCKET_ERROR) {
                std::cout << std::format("[ERROR] send() failed: {0}\n", WSAGetLastError());
                Stop();
                return false;
            }

            total_sent += sent;
        }

        return true;
    }

    void Stop()
    {
        m_isRunning = false;

        if (m_socket != INVALID_SOCKET) {
            closesocket(m_socket);
            m_socket = INVALID_SOCKET;
        }

        if (m_recvThread.joinable()) {
            m_recvThread.join();
        }
    }
 private:
    void ReceiveLoop()
    {
        const int BUF_SIZE = 1024;
        char buffer[BUF_SIZE];

        while (m_isRunning) {
            int recv_bytes = recv(m_socket, buffer, BUF_SIZE - 1, 0);

            if (recv_bytes > 0) {
                buffer[recv_bytes] = '\0';
                std::cout << std::format("[Server]: {0}\n", buffer);
            } else if (recv_bytes == 0) {
                std::cout << "[INFO] Server disconnected.\n";
                m_isRunning = false;
                break;
            } else {
                int err = WSAGetLastError();

                if (m_isRunning && err != WSAEINTR) {
                    std::cout << std::format("[ERROR] recv() failed: {0}\n", err);
                }

                m_isRunning = false;
                break;
            }
        }
    }

    SOCKET m_socket = INVALID_SOCKET;

    UINT16 m_port;
    SOCKADDR_IN m_serverAddr;

    bool m_isRunning = false;
    std::thread m_recvThread;
};

#endif