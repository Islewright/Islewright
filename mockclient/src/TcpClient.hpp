#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <iostream>
#include <format>
#include <thread>


class TcpClient
{
    public:
    TcpClient() = default;

    ~TcpClient() = default;
    
    bool Init(UINT16 port)
    {
        socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(socket_ == INVALID_SOCKET)   
        {
            std::string msg = std::format("[ERROR] socket() failed: {0}\n", WSAGetLastError());
            std::cout << msg;
            return false;
        }
        
        port_ = port;
        
        return true;
    }

    bool Connect(const std::string serverIP = "127.0.0.1")
    {
        int ret;
        memset(&serverAddr, 0x00, sizeof(SOCKADDR_IN));
        serverAddr.sin_family = AF_INET;
        ret = InetPtonA(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);
        serverAddr.sin_port = htons(port_);
        ret = connect(socket_, (struct sockaddr *)&serverAddr, sizeof(SOCKADDR_IN));
        if(ret == SOCKET_ERROR)
        {
            std::string msg = std::format("[ERROR] connect() failed: {0}\n", WSAGetLastError());
            std::cout << msg;
            return false;
        }

        std::cout << "[SYSTEM] Server connected success\n";
        
        isRunning_ = true;
        recvThread_ = std::thread([this]() {ReceiveLoop();});
        return true;
    }

    bool SendMessage(const std::string& message)
    {
        if (socket_ == INVALID_SOCKET || !isRunning_) return false;

        int total_sent = 0;
        int msg_len = static_cast<int>(message.length());

        // TCP 특성상 데이터가 한 번에 다 안 갈 수도 있으므로 루프 처리
        while (total_sent < msg_len)
        {
            int sent = send(socket_, message.c_str() + total_sent, msg_len - total_sent, 0);
            if (sent == SOCKET_ERROR)
            {
                std::cout << std::format("[ERROR] send() failed: {0}\n", WSAGetLastError());
                Stop(); // 송신 실패 시 연결 종료 처리
                return false;
            }
            total_sent += sent;
        }
        return true;
    }

    void Stop()
    {
        isRunning_ = false;

        // 소켓을 닫아 무한 대기 중인 recv() 블로킹을 깨웁니다.
        if (socket_ != INVALID_SOCKET)
        {
            closesocket(socket_);
            socket_ = INVALID_SOCKET;
        }

        // 수신 스레드가 종료될 때까지 대기
        if (recvThread_.joinable())
        {
            recvThread_.join();
        }
    }


    private:

    void ReceiveLoop()
    {
        const int BUF_SIZE = 1024;
        char buffer[BUF_SIZE];

        while (isRunning_)
        {
            // 서버로부터 데이터 수신 대기 (블로킹 상태)
            int recv_bytes = recv(socket_, buffer, BUF_SIZE - 1, 0);

            if (recv_bytes > 0)
            {
                buffer[recv_bytes] = '\0'; // 문자열 끝 처리
                std::cout << std::format("[Server]: {0}\n", buffer);
            }
            else if (recv_bytes == 0)
            {
                // recv가 0을 반환하면 서버가 정상적으로 연결을 종료한 것임
                std::cout << "[INFO] Server disconnected.\n";
                break;
            }
            else
            {
                // 소켓이 외부(Stop 함수 등)에서 닫히면 WSAEINTR(10004) 등이 발생할 수 있음
                int err = WSAGetLastError();
                if (isRunning_ && err != WSAEINTR)
                {
                    std::cout << std::format("[ERROR] recv() failed: {0}\n", err);
                }
                break;
            }
        }
    }

    SOCKET socket_ = INVALID_SOCKET;

    UINT16 port_;

    SOCKADDR_IN serverAddr;

    bool isRunning_ = false;

    std::thread recvThread_;
};

#endif