#ifndef ISLEWRIGHTERSERVER_H
#define ISLEWRIGHTERSERVER_H

#include "clientInfo.hpp"
#include <vector>
#include <thread> 
#include <WinSock2.h>
#include <windows.h>
#include <iostream>
#include <format>


class IslewrighterServer{
    public:
    IslewrighterServer()
    {
        WSAStartup(MAKEWORD(2,2), &wsa_);
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

    void StartServer()
    {
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

    }

    private:

    void AccepterThread()
    {
        int addrlen = sizeof(SOCKADDR_IN);
        while(isAccepterRun_)
        {
            SOCKET acceptedSocket = accept(listenSocket_, (struct sockaddr*) &serverAddr, &addrlen);
            if(acceptedSocket == INVALID_SOCKET)
            {
                // TODO: Must add error handle routines like Shutdown server or ReIntialize listening socket....
                std::string errMsg = std::format("ERROR] accept() failed: {0}\n", WSAGetLastError());
                std::cout << errMsg;
                break;
            }
            
            AddClient(acceptedSocket);
        }
    }

    void AddClient(SOCKET socket)
    {
        std::unique_ptr<stClientInfo> newClient = std::make_unique<stClientInfo>();
        newClient->socket_ = socket;
        newClient->clientIndex_ = clientCount_++;
        clientInfos_.push_back(std::move(newClient));
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

    bool isAccepterRun_ = false;

    std::thread accepterThread_;

    bool isSocketHandlerRun_ = false;

    UINT64 clientCount_ = 0;

    std::vector<std::unique_ptr<stClientInfo>> clientInfos_;
};

#endif