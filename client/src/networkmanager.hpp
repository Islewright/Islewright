#ifndef ISLEWRIGHT_NETWORKMANAGER_HPP
#define ISLEWRIGHT_NETWORKMANAGER_HPP

#include "tcpconnector.hpp"

#include <format>
#include <iostream>
#include <queue>
#include <vector>
#include <string>

namespace islewright::networkmanager {

using TcpConnector = islewright::tcpconnector::TcpConnector;

class NetworkManager : public TcpConnector
{
  public:
    void OnConnect() override 
    { 
        std::cout << "[CONNECT] Server connected success\n";
    }

    // Network receive callback handler
    void OnReceive(char* message, int len) override
    {
        if (message == nullptr || len <= 0) return;

        std::string log = std::format("[RECV] Length: {0}, Data: {1}\n", len, std::string_view(message, len));
        std::cout << log; 
        
        std::vector<char> packet(len);
        std::memcpy(packet.data(), message, len);

        m_receivedQueue.push(std::move(packet));
    }

    void OnDisConnect() override
    {
        std::cout << "[DISCONNECT] Server Disconnected\n";
    }
  private:
    std::queue<std::vector<char>> m_receivedQueue;
};

} // namespace islewright::networkmanager

# endif // ISLEWRIGHT_NETWORKMANAGER_HPP