#ifndef ISLEWRIGHTERSERVER_H
#define ISLEWRIGHTERSERVER_H

#include "clientInfo.hpp"
#include <vector>
#include <thread> 
#include <WinSock2.h>
#include <windows.h>
#include <iostream>
#include <format>
#include <stdexcept> 

class IslewrighterServer{
    public:
    IslewrighterServer()
    {
        if (WSAStartup(MAKEWORD(2,2), &wsa_) != 0)
        {
            throw std::runtime_error("WSAStartup failed");
        }
    }

    ~IslewrighterServer()
    {
        WSACleanup();
    }

    bool BindAndListen(UINT16 u16Port) 
    {
        port_ = u16Port;

        listenSocket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(listenSocket_ == INVALID_SOCKET)
        {
            std::cout << "[ERROR] socket() failed\n";
            return false;
        }

        ZeroMemory(&serverAddr, sizeof(SOCKADDR_IN));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port_);
        serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

        int ret = bind(listenSocket_, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN));
        if(ret == SOCKET_ERROR)
        {
            
            std::string errMsg = std::format("ERROR] bind() failed: {0}\n", ret);
            std::cout << errMsg;
            CloseSocket();
            return false;
        }

        ret = listen(listenSocket_, 5);
        if(ret == SOCKET_ERROR)
        {
            
            std::string errMsg = std::format("ERROR] listen() failed: {0}\n", ret);
            std::cout << errMsg;
            CloseSocket();
            return false;
        }

        std::cout << "[SYSTEM] Listening socket bind and listen success\n";
        return true;
    }

    void StartServer(UINT32 clientCount = 50)
    {
        CreateClient(clientCount);

        isAccepterRun_ = true;
        accepterThread_ = std::thread( [this]() { AccepterThread(); } );
        
        std::cout << "[SYSTEM] AcceptThread created\n";
        std::cout << "[SYSTEM] Server stated\n";
    }

    void EndServer()
    {
        isAccepterRun_ = false;

        CloseSocket();
        
        if(accepterThread_.joinable())
        {
            accepterThread_.join();
        }

        for(int i = 0; i < maxClientCount_; i++)
        {
            if(clientInfos_[i].socket_ == INVALID_SOCKET) continue;
            stClientInfo* client = &clientInfos_[i];
            closesocket(client->socket_);
            if(client->recvThread_.joinable())
            {
                client->recvThread_.join();
            }
        }
    }

    private:
    void CreateClient(UINT32 clientCount)
    {
        maxClientCount_ = clientCount;
        for(int i = 0; i < clientCount; i++)
        {
            stClientInfo newClient;
            newClient.clientIndex_ = i;
            clientInfos_.push_back(std::move(newClient));
        }
    }

    stClientInfo* GetEmptyClient()
    {
        for(auto & client: clientInfos_)
        {
            if(client.socket_ == INVALID_SOCKET) return &client;
        }
        return nullptr;
    }

    void AccepterThread()
    {
        while(isAccepterRun_)
        {
            SOCKADDR_IN clientAddr;
            SOCKET acceptedSocket = accept(listenSocket_, (struct sockaddr*) &clientAddr, &addrlen);
            if(acceptedSocket == INVALID_SOCKET)
            {
                // TODO: Must add error handle routines like Shutdown server or ReIntialize listening socket....
                std::string errMsg = std::format("ERROR] accept() failed: {0}\n", WSAGetLastError());
                std::cout << errMsg;
                break;
            }

            // TODO: Thread Safety
            stClientInfo* client = GetEmptyClient();
            if(client == nullptr)
            {
                std::cout << "[WARN] Client pool full, connection rejected\n";
                closesocket(acceptedSocket);
                continue;
            }
            client->socket_ = acceptedSocket;
            client->clientAddr_ = std::move(clientAddr);
            client->recvThread_ = std::thread(&IslewrighterServer::RecvThread, this, client);
        }
    }

    void RecvThread(stClientInfo* client)
    {
        char buf[MAX_BUF_SIZE];
        while(true)
        {
            int transferredSize = recv(client->socket_, buf, MAX_BUF_SIZE, 0);
            if(transferredSize == 0)
            {
                std::string msg = std::format("[SYSTEM] client {0} disconnected\n", client->clientIndex_);
                std::cout << msg;
                break;
            }
            else if(transferredSize < 0)
            {
                std::string msg = std::format("[ERROR] client {0} recv() failed: {1}\n", client->clientIndex_, WSAGetLastError());
                std::cout << msg;
                break;
            }

            buf[transferredSize] = '\0';
            std::string msg = std::format("[RECV][Client {0}] {1}, {2}bytes\n", client->clientIndex_, buf, transferredSize);
            std::cout << msg;

            // NOTE: Echo Test
            send(client->socket_, buf, transferredSize, 0);
            
            ZeroMemory(buf, sizeof(buf));
        }

        std::string msg = std::format("[RECV][Client {0}] Recv thread ended\n", client->clientIndex_);
        std::cout << msg;

        closesocket(client->socket_);
        client->socket_ = INVALID_SOCKET;
    }

    bool Send(SOCKET socket, const char* pData, int transferredSize)
    {
        if(socket == INVALID_SOCKET) return false;
        int totalSent = 0;
        while(totalSent < transferredSize)
        {
            int ret = send(socket, pData + totalSent, transferredSize - totalSent, 0);
            if(ret == SOCKET_ERROR)
            {
                std::string msg = std::format("[ERROR] send() failed: {0}]n", WSAGetLastError());
                std::cout << msg;
                return false;
            }
            totalSent += ret;
        }

        return true;
    }

    void CloseSocket()
    {
        // NOTE: Must consider about graceful close.
        closesocket(listenSocket_);
        listenSocket_ = INVALID_SOCKET;
    }

    WSAData wsa_;

    SOCKET listenSocket_ = INVALID_SOCKET;

    UINT16 port_ = 9000;

    SOCKADDR_IN serverAddr;

    int addrlen = sizeof(SOCKADDR_IN);  

    bool isAccepterRun_ = false;

    std::thread accepterThread_;

    bool isSocketHandlerRun_ = false;

    UINT64 maxClientCount_ = 0;

    std::vector<stClientInfo> clientInfos_;
    
};

#endif