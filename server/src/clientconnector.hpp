#ifndef ISLEWRIGHT_CLIENTCONNECTOR_HPP
#define ISLEWRIGHT_CLIENTCONNECTOR_HPP

#include "clientinfo.hpp"

#include <WS2tcpip.h>
#include <WinSock2.h>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace islewright::clientconnector {

using ClientInfo = islewright::clientinfo::ClientInfo;

class ClientConnector
{
  public:
    ClientConnector(USHORT port = 9000) : m_port(port)
    {
        WSADATA wsaData;
        int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);

        if (ret != 0) {
            throw std::runtime_error("[ERROR] WSAStartup failed with error: " +
                                     std::to_string(ret));
        }
    }

    virtual ~ClientConnector()
    {
        EndNetworking();
        CloseListenSocket();

        delete m_clientInfo;
        m_clientInfo = nullptr;

        WSACleanup();
    }

    // Getters
    USHORT GetPort() const noexcept
    {
        return m_port;
    }

    // Setters
    void SetPort(USHORT port) noexcept
    {
        m_port = port;
    }

    // Create server socket and start listening
    bool Listen()
    {
        m_listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if (m_listenSocket == INVALID_SOCKET) {
            std::cout << "[ERROR] socket() failed:" << WSAGetLastError() << "\n";
            return false;
        }

        SOCKADDR_IN serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        serverAddr.sin_port = htons(m_port);

        int ret =
            bind(m_listenSocket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(serverAddr));

        if (ret == SOCKET_ERROR) {
            std::cout << "[ERROR] bind() failed:" << WSAGetLastError() << "\n";
            CloseListenSocket();
            return false;
        }

        ret = listen(m_listenSocket, 1);

        if (ret == SOCKET_ERROR) {
            std::cout << "[ERROR] listen() failed:" << WSAGetLastError() << "\n";
            CloseListenSocket();
            return false;
        }

        return true;
    }

    // Wait for one client connection
    bool Accept()
    {
        if (m_listenSocket == INVALID_SOCKET || m_clientInfo != nullptr) {
            return false;
        }

        SOCKADDR_IN clientAddr{};
        int clientAddrSize = sizeof(clientAddr);

        SOCKET clientSocket =
            accept(m_listenSocket, reinterpret_cast<SOCKADDR*>(&clientAddr), &clientAddrSize);

        if (clientSocket == INVALID_SOCKET) {
            std::cout << "[ERROR] accept() failed:" << WSAGetLastError() << "\n";
            return false;
        }

        m_clientInfo = new ClientInfo(clientSocket, clientAddr);

        OnConnect();

        return true;
    }

    // Start receive and send data from client
    void StartNetworking()
    {
        if (m_clientInfo == nullptr) {
            return;
        }

        m_isNetworking = true;
        CreateRecvThread();
    }

    void EndNetworking()
    {
        m_isNetworking = false;

        {
            std::lock_guard<std::mutex> lock(m_sendMutex);

            if (m_clientInfo != nullptr && m_clientInfo->socket != INVALID_SOCKET) {
                shutdown(m_clientInfo->socket, SD_BOTH);
            }
        }

        if (m_recvThread.joinable()) {
            m_recvThread.join();
        }

        CloseClientSocket();
    }

    bool Send(const char* msg, const int len)
    {
        if (msg == nullptr || len <= 0 || len > ClientInfo::BUFFER_SIZE) {
            return false;
        }

        std::lock_guard<std::mutex> lock(m_sendMutex);

        if (!m_isNetworking || m_clientInfo == nullptr ||
            m_clientInfo->socket == INVALID_SOCKET) {
            return false;
        }

        const uint32_t networkLength = htonl(static_cast<uint32_t>(len));

        if (!SendAll(reinterpret_cast<const char*>(&networkLength), sizeof(networkLength))) {
            return false;
        }

        return SendAll(msg, len);
    }

    virtual void OnConnect() = 0;

    virtual void OnReceive(char* message, int len) = 0;

    virtual void OnDisconnect() = 0;

  private:
    bool SendAll(const char* data, int len)
    {
        int totalSent = 0;

        while (totalSent < len) {
            const int sent = send(m_clientInfo->socket, data + totalSent, len - totalSent, 0);

            if (sent == SOCKET_ERROR || sent == 0) {
                std::cout << "[ERROR] send() failed:" << WSAGetLastError() << "\n";
                return false;
            }

            totalSent += sent;
        }

        return true;
    }

    bool ProcessReceivedData(const char* data, int len)
    {
        m_receiveAccumulator.insert(m_receiveAccumulator.end(), data, data + len);

        constexpr std::size_t HEADER_SIZE = sizeof(uint32_t);

        while (m_receiveAccumulator.size() >= HEADER_SIZE) {
            uint32_t networkLength = 0;
            std::memcpy(&networkLength, m_receiveAccumulator.data(), HEADER_SIZE);

            const uint32_t payloadLength = ntohl(networkLength);
            if (payloadLength == 0 || payloadLength > ClientInfo::BUFFER_SIZE) {
                std::cout << "[ERROR] Invalid payload length:" << payloadLength << "\n";
                return false;
            }

            const std::size_t frameSize = HEADER_SIZE + payloadLength;
            if (m_receiveAccumulator.size() < frameSize) {
                return true;
            }

            OnReceive(m_receiveAccumulator.data() + HEADER_SIZE, static_cast<int>(payloadLength));
            m_receiveAccumulator.erase(m_receiveAccumulator.begin(),
                                       m_receiveAccumulator.begin() + frameSize);
        }

        return true;
    }

    void CloseListenSocket()
    {
        if (m_listenSocket == INVALID_SOCKET) {
            return;
        }

        closesocket(m_listenSocket);
        m_listenSocket = INVALID_SOCKET;
    }

    void CloseClientSocket()
    {
        if (m_clientInfo == nullptr || m_clientInfo->socket == INVALID_SOCKET) {
            return;
        }

        closesocket(m_clientInfo->socket);
        m_clientInfo->socket = INVALID_SOCKET;
    }

    void CreateRecvThread()
    {
        m_recvThread = std::thread([this]() { Recv(); });
    }

    void Recv()
    {
        while (m_isNetworking) {
            int ret = recv(m_clientInfo->socket, m_clientInfo->recvBuffer,
                           ClientInfo::BUFFER_SIZE, 0);

            if (ret > 0) {
                if (!ProcessReceivedData(m_clientInfo->recvBuffer, ret)) {
                    m_isNetworking = false;
                }
            } else if (ret == 0) {
                std::cout << "[RECV] Client connection closed\n";
                m_isNetworking = false;
            } else {
                if (m_isNetworking) {
                    std::cout << "[ERROR] recv() failed:" << WSAGetLastError() << "\n";
                }

                m_isNetworking = false;
            }
        }

        OnDisconnect();
    }

    SOCKET m_listenSocket = INVALID_SOCKET;
    USHORT m_port;

    ClientInfo* m_clientInfo = nullptr;

    std::atomic_bool m_isNetworking = false;

    std::thread m_recvThread;
    std::mutex m_sendMutex;
    std::vector<char> m_receiveAccumulator;
};

} // namespace islewright::clientconnector

#endif // ISLEWRIGHT_CLIENTCONNECTOR_HPP
