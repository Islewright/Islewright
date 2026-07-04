#ifndef CLIENT_INFO_H
#define CLIENT_INFO_H

#include "Define.hpp"

class ClientInfo
{
    public:
    ClientInfo() = default;
    
    ~ClientInfo() = default;

    void Init(const UINT32 u32ClientIndex, const HANDLE hIOCP) 
    {
        clientIndex_ = u32ClientIndex;

        iocp_ = hIOCP;
    }

    void InitSocket(const SOCKET hClientSocket)
    {
        socket_ = hClientSocket;
        BindIOCP();
        isConnected = true;

    }

    bool IsConnected() { return isConnected; }

    UINT32 GetIndex() { return clientIndex_; }

    void DisConnect() {}

    bool BindIOCP() 
    {
        HANDLE result = CreateIoCompletionPort((HANDLE)socket_, iocp_, 0, 0);
        if(result == NULL)
        {
            std::cerr << "[ERROR] BindIOCP(): " << WSAGetLastError() << std::endl;
            // TODO: Error Control Routine
            return false;
        }

        return true;
    }

    void RegisterReceive() 
    {
        OverlappedEx* overlappedEx = new OverlappedEx();
        overlappedEx->clientIndex = clientIndex_;
        overlappedEx->operation = IOOperation::RECV;
        
        DWORD flags = 0;
        DWORD bytesReceived = 0;

        int result = WSARecv(
            clientSocket_, 
            &(overlappedEx->wsabuf), 1, 
            &bytesReceived,  &flags, 
            &(overlappedEx->overlapped), 
            NULL
        );
        if(result == SOCKET_ERROR)
        {
            int errorCode = WSAGetLastError();
            if (errorCode != WSA_IO_PENDING)
            {
                std::cout << std::format("WSARecv() Failed: {0}\n", errorCode);
                delete overlappedEx;
            }
        }
    }

    void BindSend() {}


    private:
    UINT32 clientIndex_;
    
    SOCKET clientSocket_;

    HANDLE iocp_;

    SOCKET socket_ = INVALID_SOCKET;

    bool isConnected = false;
    
};

#endif