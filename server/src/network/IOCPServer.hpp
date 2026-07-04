#ifndef IOCP_SERVER_H
#define IOCP_SERVER_H


#include "ClientInfo.hpp"
#include <thread>
#include <vector>
#include <print>


class IOCPServer 
{
    public:
    IOCPServer() = default;
    ~IOCPServer() { WSACleanup(); }

    bool Init(const UINT16 u16Port, const UINT16 u16WorkerCount)  
    {
        port_ = u16Port;
        workerCount_ = u16WorkerCount;

        int ret = WSAStartup(MAKEWORD(2, 2), &wsaData_);
        if (ret != 0) 
        {
            std::cout << "[ERROR] WSAStartup() failed with error: " << ret << "\n";
            return false;
        }
        
        return true;
    }



    /// @brief  Bind&Listen listening socket
    bool BindAndListen() 
    {
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
            std::cout << "[ERROR] bind() failed\n";
            CloseSocket();
            return false;
        }

        ret = listen(listenSocket_, 5);
        if(ret == SOCKET_ERROR)
        {
            std::cout << "[ERROR] listen() failed\n";
            CloseSocket();
            return false;
        }

        std::cout << "[SYSTEM] Listening socket bind and listen success\n";
        return true;
    }

    /// @brief Create IOCPHandle/ClientSession/AccepterThread/WorkerThread
    bool StartServer(const UINT32 maxClient) 
    {
        
        iocp_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL , 0, workerCount_);
        if (iocp_ == NULL) 
        {
            std::cerr << "[ERROR] CreateIoCompletionPort(): " << GetLastError() << std::endl;
            // TODO: Error Contol

            return false;
        }

        
        CreateClient(maxClient);

        isWorkerRun_ = true;
        CreateWorkerThread();

        isAccepterRun_ = true;
        CreateAccepterThread();

        return true;
    }

    void DestroyThread() 
    {
        CloseSocket();
        isAccepterRun_ = false;
        isWorkerRun_ = false;
        
        if(accepterThread_.joinable())
        {
            accepterThread_.join();
        }

        for(auto & e: workerThreads_)
        {
            if(e.joinable())
            {
                e.join();
            }
        }
    }
    
    // Network IO Callback overrided at business logic
    virtual void OnConnect(const UINT32 u32ClientIndex) {}
    
    virtual void OnDisConnect() {}

    virtual void OnRecieve() {}

    virtual void OnSend() {}

    private:

    void CreateClient(const UINT32 maxClient) 
    {
        maxClientCount_ = maxClient;
        for(UINT32 i = 0; i < maxClientCount_; i++)
        {
            ClientInfo* newClient = new ClientInfo();
            newClient->Init(i, iocp_);
            clientInfos_.push_back(newClient);
        }
    }

    void CreateAccepterThread() 
    {
        accepterThread_ = std::thread([this]() {AccepterThread();});
        
        std::cout << "[SYSTEM] AccpterThread Created\n";
        return;
    }

    void AccepterThread() 
    {
        int addrlen = sizeof(SOCKADDR_IN);
        while(isAccepterRun_)
        {
            SOCKADDR_IN clientAddr;
            ZeroMemory(&clientAddr, sizeof(SOCKADDR_IN));
            SOCKET newSocket = accept(listenSocket_, (struct sockaddr*)&clientAddr, &addrlen);
            if(newSocket == INVALID_SOCKET)
            {
                std::cerr << "[ERROR] accept(): " << WSAGetLastError() << std::endl;
                // TODO: Error Contol
                return;
            }

            // TODO: Discuss about thread lock
            ClientInfo* client = GetEmptyClient();
            client->InitSocket(newSocket);
            client->RegisterReceive();
            
            OnConnect(client->GetIndex());
        }
    }

    void CreateWorkerThread() 
    {
        for(UINT16 i = 0; i < workerCount_; i ++)
        {
            workerThreads_.push_back(std::thread( [this]() { WorkerThread(); }));
        }
        
        std::cout << "[SYSTEM] WorkerThread Created\n";
    }

    // FIXME: Memory leakage possibility exists. Care about freeing OverlappedEx.
    // If the IO operation fails or is canceled, OverlappedEx must be deleted in the error handler.
    void WorkerThread() 
    {
        DWORD bytesTransferred = 0;
        ULONG_PTR completionKey = 0;
        LPOVERLAPPED overlapped = nullptr;

        while(isWorkerRun_)
        {
            BOOL success = GetQueuedCompletionStatus(
                iocp_,
                &bytesTransferred,
                &completionKey,
                &overlapped,
                INFINITE
            );

            OverlappedEx* overlappedEx = (OverlappedEx*)overlapped;

            if (!success)
            {

            }

            // Client Connection Closed
            if (bytesTransferred = 0)
            {
                clientInfos_[overlappedEx->clientIndex]->DisConnect();

                delete overlappedEx;
                
                continue;
            }
        }
    }

    ClientInfo* GetEmptyClient() 
    {
        for(UINT32 i = 0; i < maxClientCount_; i++)
        {
            if(!clientInfos_[i]->IsConnected())
            {
                return clientInfos_[i];
            }
        }

        return nullptr;
    }

    void CloseSocket()
    {
        closesocket(listenSocket_);
        listenSocket_ = INVALID_SOCKET;
    }

    
    WSADATA wsaData_;
    
    UINT16 port_ = -1;

    UINT32 maxClientCount_ = 1000;

    UINT16 workerCount_ = 8;
    
    SOCKADDR_IN serverAddr;

    SOCKET listenSocket_ = INVALID_SOCKET;

    HANDLE iocp_ = NULL;

    std::vector<ClientInfo*> clientInfos_;

    bool isAccepterRun_ = false;

    std::thread accepterThread_;

    bool isWorkerRun_ = false;
    
    std::vector<std::thread> workerThreads_;

};

#endif