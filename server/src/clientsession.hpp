#ifndef ISLEWRIGHT_CLIENTSESSION_HPP
#define ISLEWRIGHT_CLIENTSESSION_HPP

#include "clientconnector.hpp"

#include <cstring>
#include <format>
#include <iostream>
#include <queue>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace islewright::clientsession {

using ClientConnector = islewright::clientconnector::ClientConnector;

class ClientSession : public ClientConnector
{
  public:
    using ClientConnector::ClientConnector;

    void OnConnect() override
    {
        std::cout << "[CONNECT] Client connected success\n";
    }

    void OnReceive(char* message, int len) override
    {
        if (message == nullptr || len <= 0) {
            return;
        }

        std::string log =
            std::format("[RECV] Length: {0}, Data: {1}\n", len, std::string_view(message, len));
        std::cout << log;

        std::vector<char> packet(len);
        std::memcpy(packet.data(), message, len);

        m_receivedQueue.push(std::move(packet));

        Send(message, len);
    }

    void OnDisconnect() override
    {
        std::cout << "[DISCONNECT] Client disconnected\n";
    }

  private:
    std::queue<std::vector<char>> m_receivedQueue;
};

} // namespace islewright::clientsession

#endif // ISLEWRIGHT_CLIENTSESSION_HPP