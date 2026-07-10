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

class IslewrightServer{
    public:
    IslewrightServer()
    {
        WSAData wsa;
        if (WSAStartup(MAKEWORD(2,2), &wsa) != 0)
        {
            throw std::runtime_error("WSAStartup failed");
        }
    }

    ~IslewrightServer()
    {
        WSACleanup();
    }

    bool BindAndListen(UINT16 u16Port) 
    {
        port_ = u16Port;

        m_listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(m_listenSocket == INVALID_SOCKET)
        {
            std::cout << "[ERROR] socket() failed\n";
            return false;
        }

        ZeroMemory(&serverAddr, sizeof(SOCKADDR_IN));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port_);
        serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

        int ret = bind(m_listenSocket, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN));
        if(ret == SOCKET_ERROR)
        {
            
            std::string errMsg = std::format("ERROR] bind() failed: {0}\n", ret);
            std::cout << errMsg;
            CloseSocket();
            return false;
        }

        ret = listen(m_listenSocket, 5);
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

        m_isAccepterRun = true;
        m_accepterThread = std::thread( [this]() { AccepterThread(); } );
        
        std::cout << "[SYSTEM] AcceptThread created\n";
        std::cout << "[SYSTEM] Server stated\n";
    }

    void EndServer()
    {
        m_isAccepterRun = false;

        CloseSocket();
        
        if(m_accepterThread.joinable())
        {
            m_accepterThread.join();
        }

        for(int i = 0; i < m_maxClientCount; i++)
        {
            if(m_clientInfos[i].socket == INVALID_SOCKET) continue;
            stClientInfo* client = &m_clientInfos[i];
            closesocket(client->socket);
            if(client->recvThread.joinable())
            {
                client->recvThread.join();
            }
        }
    }

    private:
    void CreateClient(UINT32 clientCount)
    {
        m_maxClientCount = clientCount;
        for(int i = 0; i < clientCount; i++)
        {
            stClientInfo newClient;
            newClient.clientIndex = i;
            m_clientInfos.push_back(std::move(newClient));
        }
    }

    stClientInfo* GetEmptyClient()
    {
        for(auto & client: m_clientInfos)
        {
            if(client.socket == INVALID_SOCKET) return &client;
        }
        return nullptr;
    }

    void AccepterThread()
    {
        while(m_isAccepterRun)
        {
            SOCKADDR_IN clientAddr;
            SOCKET acceptedSocket = accept(m_listenSocket, (struct sockaddr*) &clientAddr, &m_addrlen);
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
            client->socket = acceptedSocket;
            client->clientAddr = std::move(clientAddr);
            client->recvThread = std::thread(&IslewrightServer::RecvThread, this, client);
        }
    }

    void RecvThread(stClientInfo* client)
    {
        char buf[MAX_BUF_SIZE];
        while(true)
        {
            int transferredSize = recv(client->socket, buf, MAX_BUF_SIZE, 0);
            if(transferredSize == 0)
            {
                std::string msg = std::format("[SYSTEM] client {0} disconnected\n", client->clientIndex);
                std::cout << msg;
                break;
            }
            else if(transferredSize < 0)
            {
                std::string msg = std::format("[ERROR] client {0} recv() failed: {1}\n", client->clientIndex, WSAGetLastError());
                std::cout << msg;
                break;
            }

            buf[transferredSize] = '\0';
            std::string msg = std::format("[RECV][Client {0}] {1}, {2}bytes\n", client->clientIndex, buf, transferredSize);
            std::cout << msg;

            // NOTE: Echo Test
            send(client->socket, buf, transferredSize, 0);
            
            ZeroMemory(buf, sizeof(buf));
        }

        std::string msg = std::format("[RECV][Client {0}] Recv thread ended\n", client->clientIndex);
        std::cout << msg;

        closesocket(client->socket);
        client->socket = INVALID_SOCKET;
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
        closesocket(m_listenSocket);
        m_listenSocket = INVALID_SOCKET;
    }

    SOCKET m_listenSocket = INVALID_SOCKET;

    UINT16 port_ = 9000;

    SOCKADDR_IN serverAddr;

    int m_addrlen = sizeof(SOCKADDR_IN);  

    bool m_isAccepterRun = false;

    std::thread m_accepterThread;

    bool m_isSocketHandlerRun = false;

    UINT64 m_maxClientCount = 0;

    std::vector<stClientInfo> m_clientInfos;
    
};

#endif