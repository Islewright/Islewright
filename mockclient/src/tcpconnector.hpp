#ifndef ISLEWRIGHT_TCPCONNECTOR_HPP
#define ISLEWRIGHT_TCPCONNECTOR_HPP

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

namespace islewright::tcpconnector {

class TcpConnector
{
  public:
    TcpConnector(const std::string hostAddr = "127.0.0.1", const USHORT port = 9000)
        : m_serverAddr(hostAddr), m_port(port)
    {
        WSADATA wsaData;
        int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (ret != 0) {
            throw std::runtime_error("[ERROR] WSAStartup failed with error: " +
                                     std::to_string(ret));
        }

        m_recvBuffer = new char[BUFFER_SIZE + 1];
        m_sendBuffer = new char[BUFFER_SIZE + 1];
    }

    virtual ~TcpConnector()
    {
        EndNetworking();

        delete[] m_recvBuffer;
        delete[] m_sendBuffer;

        WSACleanup();
    }

    // Getters
    std::string GetHost()
    {
        return m_serverAddr;
    }

    USHORT GetPort()
    {
        return m_port;
    }

    // Setters
    void SetHost(std::string hostAddr)
    {
        m_serverAddr = hostAddr;
    }

    void SetPort(USHORT port)
    {
        m_port = port;
    }

    // Connect to Server
    bool Connect()
    {
        m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (m_socket == INVALID_SOCKET) {
            std::cout << "[ERROR] socket() failed:" << WSAGetLastError() << "\n";
            return false;
        }

        SOCKADDR_IN serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(m_port);
        inet_pton(AF_INET, m_serverAddr.c_str(), &serverAddr.sin_addr);

        int ret = connect(m_socket, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN));
        if (ret == SOCKET_ERROR) {
            CloseSocket();
            return false;
        }

        OnConnect();

        return true;
    }

    // Start receive and send data from server
    void StartNetworking()
    {
        m_isNetworking = true;
        CreateRecvThread();
    }

    void EndNetworking()
    {
        m_isNetworking = false;

        {
            std::lock_guard<std::mutex> lock(m_sendMutex);

            if (m_socket != INVALID_SOCKET) {
                shutdown(m_socket, SD_BOTH);
            }
        }

        if (m_recvThread.joinable()) {
            m_recvThread.join();
        }

        if (m_socket != INVALID_SOCKET) {
            CloseSocket();
        }
    }

    bool Send(const char* msg, const int len)
    {
        if (msg == nullptr || len <= 0 || len > BUFFER_SIZE) {
            return false;
        }

        std::lock_guard<std::mutex> lock(m_sendMutex);

        if (!m_isNetworking || m_socket == INVALID_SOCKET) {
            return false;
        }

        const uint32_t networkLength = htonl(static_cast<uint32_t>(len));

        if (!SendAll(reinterpret_cast<const char*>(&networkLength), sizeof(networkLength))) {
            return false;
        }

        return SendAll(msg, len);
    }

    virtual void OnConnect() {}

    virtual void OnReceive(char* message, int len) {}

    virtual void OnDisconnect() {}

  private:
    bool SendAll(const char* data, int len)
    {
        int totalSent = 0;

        while (totalSent < len) {
            const int sent = send(m_socket, data + totalSent, len - totalSent, 0);

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
            if (payloadLength == 0 || payloadLength > BUFFER_SIZE) {
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

    void CloseSocket()
    {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
    }

    void CreateRecvThread()
    {
        m_recvThread = std::thread([this]() { Recv(); });
    }

    void Recv()
    {
        while (m_isNetworking) {
            int ret = recv(m_socket, m_recvBuffer, BUFFER_SIZE, 0);

            if (ret > 0) {
                if (!ProcessReceivedData(m_recvBuffer, ret)) {
                    m_isNetworking = false;
                }
            } else if (ret == 0) {
                std::cout << "[RECV] Server connection closed\n";
                m_isNetworking = false;
            } else {
                std::cout << "[ERROR] recv() failed:" << WSAGetLastError() << "\n";
                m_isNetworking = false;
            }
        }

        OnDisconnect();
    }

    SOCKET m_socket = INVALID_SOCKET;
    std::string m_serverAddr;
    USHORT m_port;

    std::atomic_bool m_isNetworking = false;

    std::thread m_recvThread;
    std::mutex m_sendMutex;
    std::vector<char> m_receiveAccumulator;

    static constexpr int BUFFER_SIZE = 1024;
    char* m_recvBuffer = nullptr;
    char* m_sendBuffer = nullptr;
};

} // namespace islewright::tcpconnector

#endif // ISLEWRIGHT_TCPCONNECTOR_HPP
