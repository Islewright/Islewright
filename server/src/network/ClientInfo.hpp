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
        isConnected_ = true;

    }

    bool IsConnected() { return isConnected_; }

    UINT32 GetIndex() { return clientIndex_; }

    char* RecvBuffer() { return recvBuffer_; }
    void DisConnect() {}

    bool BindIOCP() 
    {
        HANDLE result = CreateIoCompletionPort((HANDLE)socket_, iocp_, (ULONG_PTR)this, 0);
        if(result == NULL)
        {
            std::cerr << "[ERROR] BindIOCP(): " << WSAGetLastError() << std::endl;
            // TODO: Error Control Routine
            return false;
        }

        return true;
    }

    // TODO: 이대로 주소를 패킷큐로 보내면, 데이터가 오염되지 않을까?
    void RegisterReceive() 
    {
        DWORD flags = 0;
        DWORD bytesReceived = 0;

        recvOverlappedEx_.clientIndex = clientIndex_;
        recvOverlappedEx_.wsabuf.buf = recvBuffer_; 
        recvOverlappedEx_.wsabuf.len = MAX_BUFFER_SIZE;
        recvOverlappedEx_.operation = IOOperation::RECV;
        
        int result = WSARecv(
            socket_, 
            &(recvOverlappedEx_.wsabuf), 1, 
            &bytesReceived,  &flags, 
            &(recvOverlappedEx_.overlapped), 
            NULL
        );
        if(result == SOCKET_ERROR)
        {
            int errorCode = WSAGetLastError();
            if (errorCode != WSA_IO_PENDING)
            {
                std::cout << std::format("WSARecv() Failed: {0}\n", errorCode);
                return;
            }
        }
    }

    void BindSend(const char* pData, const UINT32 u32Size) 
    {
        if (u32Size > MAX_BUFFER_SIZE)
        {
            std::cout << "[ERROR] Send size exceeds MAX_BUFFER_SIZE\n";
            return;
        }

        memcpy(sendBuffer_, pData, u32Size);

        ZeroMemory(&sendOverlappedEx_.overlapped, sizeof(OVERLAPPED));
        sendOverlappedEx_.clientIndex = clientIndex_;
        sendOverlappedEx_.wsabuf.buf = sendBuffer_;
        sendOverlappedEx_.wsabuf.len = u32Size;
        sendOverlappedEx_.operation = IOOperation::SEND;

        DWORD bytesSent = 0;
        
        int result = WSASend(
            socket_,
            &(sendOverlappedEx_.wsabuf), 1,
            &bytesSent, 0,
            &(sendOverlappedEx_.overlapped),
            NULL
        );

        if (result == SOCKET_ERROR)
        {
            int errorCode = WSAGetLastError();
            if (errorCode != WSA_IO_PENDING)
            {
                std::cout << std::format("WSASend() Fa  iled: {0}\n", errorCode);
                // TODO: 연결 종료 처리 등의 에러 핸들링
                return;
            }
        }
    }


    private:
    UINT32 clientIndex_;

    HANDLE iocp_;

    SOCKET socket_ = INVALID_SOCKET;

    bool isConnected_ = false;

    OverlappedEx recvOverlappedEx_;
    char recvBuffer_[MAX_BUFFER_SIZE] = { 0, };
    
    OverlappedEx sendOverlappedEx_;
    char sendBuffer_[MAX_BUFFER_SIZE] = { 0, };
};

#endif