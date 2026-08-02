#ifndef ISLEWRIGHT_NETWORKMANAGER_HPP
#define ISLEWRIGHT_NETWORKMANAGER_HPP

#include "tcpconnector.hpp"
#include "islewright/common/serializer.hpp"
#include "islewright.pb.h"

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>

namespace islewright::networkmanager {

using TcpConnector = islewright::tcpconnector::TcpConnector;
using ProtobufSerializer = islewright::common::ProtobufSerializer;
using Packet = islewright::protocol::Packet;

class NetworkManager : public TcpConnector
{
  public:
    void OnConnect() override
    {
        std::cout << "[CONNECT] Server connected success\n";
    }

    bool RequestWorld(std::uint64_t seed)
    {
        Packet request;
        request.set_protocol_version(ProtocolVersion);
        request.set_request_id(m_nextRequestId++);
        request.mutable_create_world_request()->set_seed(seed);

        std::string serialized;
        if (!ProtobufSerializer::Serialize(request, serialized) || serialized.empty()) {
            return false;
        }
        return Send(serialized.data(), static_cast<int>(serialized.size()));
    }

    // Network receive callback handler
    void OnReceive(char* message, int len) override
    {
        if (message == nullptr || len <= 0) {
            return;
        }
        Packet response;
        if (!ProtobufSerializer::Deserialize(message, static_cast<std::size_t>(len), response)) {
            std::cerr << "[ERROR] Received malformed protobuf packet\n";
            return;
        }

        if (response.protocol_version() != ProtocolVersion) {
            std::cerr << "[ERROR] Unsupported protocol version: " << response.protocol_version()
                      << '\n';
            return;
        }

        if (response.has_create_world_response()) {
            std::cout << "[WORLD] Server created world with seed "
                      << response.create_world_response().seed() << " (request "
                      << response.request_id() << ")\n";
        } else if (response.has_error_response()) {
            std::cerr << "[SERVER ERROR] code=" << response.error_response().code()
                      << ", message=" << response.error_response().message() << '\n';
        } else {
            std::cerr << "[ERROR] Unexpected response payload\n";
        }
    }

    void OnDisconnect() override
    {
        std::cout << "[DISCONNECT] Server Disconnected\n";
    }

  private:
    static constexpr std::uint32_t ProtocolVersion = 1;
    std::atomic_uint64_t m_nextRequestId = 1;
};

} // namespace islewright::networkmanager

#endif // ISLEWRIGHT_NETWORKMANAGER_HPP
