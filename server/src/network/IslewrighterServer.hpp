#ifndef ISLEWRIGHTER_SERVER_H
#define ISLEWRIGHTER_SERVER_H

#include "IOCPServer.hpp"

class IslewrighterServer : public IOCPServer
{
    public:
    IslewrighterServer() {}
    ~IslewrighterServer() {}


    void Run(const UINT32 maxClient) 
    {
        StartServer(maxClient);
        std::cout << "[SYSTEM] Server Started...\n";
    }

    void End() 
    {
        DestroyThread();
    }
     
    void OnConnect(const UINT32 u32ClientIndex) override 
    {
        std::string message = std::format("Client {0} connected\n", u32ClientIndex);
        std::cout << message;
    }

    void OnDisConnect() override {}

    void OnRecieve() override {}

    void OnSend() override {}

};

#endif