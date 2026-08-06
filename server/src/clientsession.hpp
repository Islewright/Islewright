#ifndef ISLEWRIGHT_CLIENTSESSION_HPP
#define ISLEWRIGHT_CLIENTSESSION_HPP

#include "clientconnector.hpp"
#include "gameinstance.hpp"
#include "islewright/common/serializer.hpp"
#include "islewright/common/protocolversion.hpp"
#include "islewright.pb.h"

#include <cstddef>
#include <iostream>
#include <string>
#include <utility>

namespace islewright::clientsession {

using ClientConnector = islewright::clientconnector::ClientConnector;
using GameInstance = islewright::gameinstance::GameInstance;
using ProtobufSerializer = islewright::common::ProtobufSerializer;
using Packet = islewright::protocol::Packet;

class ClientSession : public ClientConnector
{
  public:
    explicit ClientSession(USHORT port = 9000)
        : ClientConnector(port)
    {
        m_gameInstance.SetResponseHandler(
            [this](const Packet& packet) { return SendPacket(packet); });
    }

    ~ClientSession() override
    {
        End();
        m_gameInstance.SetResponseHandler({});
    }

    bool Run()
    {
        if (!Listen()) {
            return false;
        }

        std::cout << "[LISTEN] Waiting for client\n";

        if (!Accept()) {
            return false;
        }

        m_gameInstance.Start();
        StartNetworking();
        return true;
    }

    void End()
    {
        EndNetworking();
        m_gameInstance.Stop();
    }

    void OnConnect() override
    {
        std::cout << "[CONNECT] Client connected success\n";
    }

    void OnReceive(char* message, int len) override
    {
        if (message == nullptr || len <= 0) {
            return;
        }

        Packet request;
        if (!ProtobufSerializer::Deserialize(message, static_cast<std::size_t>(len), request)) {
            SendError(0, islewright::protocol::ErrorResponse::INVALID_REQUEST,
                      "Malformed protobuf packet");
            return;
        }

        m_gameInstance.Enqueue(std::move(request));
    }

    void OnDisconnect() override
    {
        std::cout << "[DISCONNECT] Client disconnected\n";
    }

  private:
    bool SendPacket(const Packet& packet)
    {
        std::string serialized;
        if (!ProtobufSerializer::Serialize(packet, serialized) || serialized.empty()) {
            return false;
        }
        return Send(serialized.data(), static_cast<int>(serialized.size()));
    }

    void SendError(std::uint64_t requestId, islewright::protocol::ErrorResponse::Code code,
                   const std::string& message)
    {
        Packet response;
        response.set_protocol_version(islewright::common::PROTOCOL_VERSION);
        response.set_request_id(requestId);
        auto* error = response.mutable_error_response();
        error->set_code(code);
        error->set_message(message);

        if (!SendPacket(response)) {
            std::cerr << "[ERROR] Failed to send ErrorResponse\n";
        }
    }

    GameInstance m_gameInstance;
};

} // namespace islewright::clientsession

#endif // ISLEWRIGHT_CLIENTSESSION_HPP
