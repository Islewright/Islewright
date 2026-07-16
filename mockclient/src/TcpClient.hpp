<<<<<<< HEAD
﻿#ifndef TCPCLIENT_HPP
#define TCPCLIENT_HPP
=======
#ifndef ISLEWRIGHT_TCP_CLIENT_HPP
#define ISLEWRIGHT_TCP_CLIENT_HPP
>>>>>>> 7cbd1e5aaf3f0645bb4f289dcf3bb1ee3058c735

#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <iostream>
#include <format>
#include <thread>


class TcpClient
{
<<<<<<< HEAD
    public:
=======
 public:
>>>>>>> 7cbd1e5aaf3f0645bb4f289dcf3bb1ee3058c735
    TcpClient() = default;

    ~TcpClient()
    {
        Stop();
        
        WSACleanup();
    }
    
    bool Init(UINT16 port)
    {
        WSAData wsa;
<<<<<<< HEAD
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        {   
=======

        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
>>>>>>> 7cbd1e5aaf3f0645bb4f289dcf3bb1ee3058c735
            std::string msg = std::format("[ERROR] WSAStartup() failed: {0}\n", WSAGetLastError());
            std::cout << msg;

            return false;
        }

        m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
<<<<<<< HEAD
        if(m_socket == INVALID_SOCKET)   
        {
=======

        if (m_socket == INVALID_SOCKET) {
>>>>>>> 7cbd1e5aaf3f0645bb4f289dcf3bb1ee3058c735
            std::string msg = std::format("[ERROR] socket() failed: {0}\n", WSAGetLastError());
            std::cout << msg;

            return false;
        }
        
        m_port = port;
<<<<<<< HEAD
        
=======
>>>>>>> 7cbd1e5aaf3f0645bb4f289dcf3bb1ee3058c735
        return true;
    }

    bool Connect(const std::string serverIP = "127.0.0.1")
    {
        int ret;

        memset(&m_serverAddr, 0x00, sizeof(SOCKADDR_IN));
        m_serverAddr.sin_family = AF_INET;
        ret = InetPtonA(AF_INET, serverIP.c_str(), &m_serverAddr.sin_addr);
<<<<<<< HEAD
        if(ret != 1)
        {
            std::string msg = std::format("[ERROR] connect() failed: {0}\n", WSAGetLastError());
            return false;
        }
        m_serverAddr.sin_port = htons(m_port);

        ret = connect(m_socket, (struct sockaddr *)&m_serverAddr, sizeof(SOCKADDR_IN));
        if(ret == SOCKET_ERROR)
        {
=======

        if (ret != 1) {
            std::string msg = std::format("[ERROR] connect() failed: {0}\n", WSAGetLastError());
            return false;
        }

        m_serverAddr.sin_port = htons(m_port);
        ret = connect(m_socket, (struct sockaddr *)&m_serverAddr, sizeof(SOCKADDR_IN));

        if (ret == SOCKET_ERROR) {
>>>>>>> 7cbd1e5aaf3f0645bb4f289dcf3bb1ee3058c735
            std::string msg = std::format("[ERROR] connect() failed: {0}\n", WSAGetLastError());
            std::cout << msg;

            closesocket(m_socket);
            m_socket = INVALID_SOCKET;
<<<<<<< HEAD

=======
>>>>>>> 7cbd1e5aaf3f0645bb4f289dcf3bb1ee3058c735
            return false;
        }

        std::cout << "[SYSTEM] Server connection success\n";
<<<<<<< HEAD
        
        m_isRunning = true;
        m_recvThread = std::thread([this]() {ReceiveLoop();});

=======

        m_isRunning = true;
        m_recvThread = std::thread([this]() {ReceiveLoop();});
>>>>>>> 7cbd1e5aaf3f0645bb4f289dcf3bb1ee3058c735
        return true;
    }

    bool SendMessage(const std::string& message)
    {
<<<<<<< HEAD
        if (m_socket == INVALID_SOCKET || !m_isRunning) return false;
=======
        if (m_socket == INVALID_SOCKET || !m_isRunning) {
            return false;
        }
>>>>>>> 7cbd1e5aaf3f0645bb4f289dcf3bb1ee3058c735

        int total_sent = 0;
        int msg_len = static_cast<int>(message.length());

<<<<<<< HEAD
        // TCP 특성상 데이터가 한 번에 다 안 갈 수도 있으므로 루프 처리
        while (total_sent < msg_len)
        {
            int sent = send(m_socket, message.c_str() + total_sent, msg_len - total_sent, 0);
            if (sent == SOCKET_ERROR)
            {
                std::cout << std::format("[ERROR] send() failed: {0}\n", WSAGetLastError());
                Stop(); // 송신 실패 시 연결 종료 처리
                return false;
            }
            total_sent += sent;
        }
=======
        while (total_sent < msg_len) {
            int sent = send(m_socket, message.c_str() + total_sent, msg_len - total_sent, 0);

            if (sent == SOCKET_ERROR) {
                std::cout << std::format("[ERROR] send() failed: {0}\n", WSAGetLastError());
                Stop();
                return false;
            }

            total_sent += sent;
        }

>>>>>>> 7cbd1e5aaf3f0645bb4f289dcf3bb1ee3058c735
        return true;
    }

    void Stop()
    {
        m_isRunning = false;

<<<<<<< HEAD
        // 소켓을 닫아 무한 대기 중인 recv() 블로킹을 깨웁니다.
        if (m_socket != INVALID_SOCKET)
        {
=======
        if (m_socket != INVALID_SOCKET) {
>>>>>>> 7cbd1e5aaf3f0645bb4f289dcf3bb1ee3058c735
            closesocket(m_socket);
            m_socket = INVALID_SOCKET;
        }

<<<<<<< HEAD
        // 수신 스레드가 종료될 때까지 대기
        if (m_recvThread.joinable())
        {
            m_recvThread.join();
        }
    }


    private:

=======
        if (m_recvThread.joinable()) {
            m_recvThread.join();
        }
    }
 private:
>>>>>>> 7cbd1e5aaf3f0645bb4f289dcf3bb1ee3058c735
    void ReceiveLoop()
    {
        const int BUF_SIZE = 1024;
        char buffer[BUF_SIZE];

<<<<<<< HEAD
        while (m_isRunning)
        {
            // 서버로부터 데이터 수신 대기 (블로킹 상태)
            int recv_bytes = recv(m_socket, buffer, BUF_SIZE - 1, 0);

            if (recv_bytes > 0)
            {
                buffer[recv_bytes] = '\0'; // 문자열 끝 처리
                std::cout << std::format("[Server]: {0}\n", buffer);
            }
            else if (recv_bytes == 0)
            {
                // recv가 0을 반환하면 서버가 정상적으로 연결을 종료한 것임
                std::cout << "[INFO] Server disconnected.\n";
                m_isRunning = false;
                break;
            }
            else
            {
                // 소켓이 외부(Stop 함수 등)에서 닫히면 WSAEINTR(10004) 등이 발생할 수 있음
                int err = WSAGetLastError();
                if (m_isRunning && err != WSAEINTR)
                {
                    std::cout << std::format("[ERROR] recv() failed: {0}\n", err);
                }
=======
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

>>>>>>> 7cbd1e5aaf3f0645bb4f289dcf3bb1ee3058c735
                m_isRunning = false;
                break;
            }
        }
    }

    SOCKET m_socket = INVALID_SOCKET;

    UINT16 m_port;
<<<<<<< HEAD

    SOCKADDR_IN m_serverAddr;

    bool m_isRunning = false;

=======
    SOCKADDR_IN m_serverAddr;

    bool m_isRunning = false;
>>>>>>> 7cbd1e5aaf3f0645bb4f289dcf3bb1ee3058c735
    std::thread m_recvThread;
};

#endif