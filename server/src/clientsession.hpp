#ifndef ISLEWRIGHT_CLIENTSESSION_HPP
#define ISLEWRIGHT_CLIENTSESSION_HPP

#include "clientconnector.hpp"
#include "gameloop.hpp"
#include "islewright/common/serializer.hpp"
#include "islewright/common/world.hpp"
#include "islewright.pb.h"

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <utility>

namespace islewright::clientsession {

using ClientConnector = islewright::clientconnector::ClientConnector;
using ProtobufSerializer = islewright::common::ProtobufSerializer;
using World = islewright::common::World;
using Packet = islewright::protocol::Packet;
using GameLoop = islewright::gameloop::GameLoop;

enum class SessionState
{
    AwaitingConnection,
    AwaitingWorld,
    WorldReady,
    Disconnected
};

class ClientSession : public ClientConnector
{
  public:
    using ClientConnector::ClientConnector;

    ~ClientSession() override
    {
        EndNetworking();
    }

    void StartNetworking()
    {
        m_gameLoop.Start([this](std::uint64_t tick) { Tick(tick); });
        ClientConnector::StartNetworking();
    }

    void EndNetworking()
    {
        ClientConnector::EndNetworking();
        m_gameLoop.Stop();
        SetState(SessionState::Disconnected);
        m_world.reset();

        std::lock_guard<std::mutex> lock(m_packetMutex);
        while (!m_packets.empty()) {
            m_packets.pop();
        }
    }

    void OnConnect() override
    {
        SetState(SessionState::AwaitingWorld);
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

        std::lock_guard<std::mutex> lock(m_packetMutex);
        m_packets.push(std::move(request));
    }

    void OnDisconnect() override
    {
        std::cout << "[DISCONNECT] Client disconnected\n";
    }

  private:
    static constexpr std::uint32_t ProtocolVersion = 1;

    void Tick(std::uint64_t tick)
    {
        ProcessPendingPackets();
        UpdateWorld(tick);
    }

    void ProcessPendingPackets()
    {
        std::queue<Packet> packets;
        {
            std::lock_guard<std::mutex> lock(m_packetMutex);
            packets.swap(m_packets);
        }

        while (!packets.empty()) {
            Packet request = std::move(packets.front());
            packets.pop();

            if (request.protocol_version() != ProtocolVersion) {
                SendError(request.request_id(),
                          islewright::protocol::ErrorResponse::UNSUPPORTED_VERSION,
                          "Unsupported protocol version");
                continue;
            }

            ProcessPacket(request);
        }
    }

    void UpdateWorld(std::uint64_t tick)
    {
        if (m_state != SessionState::WorldReady || m_world == nullptr) {
            return;
        }

        // World simulation systems will be updated here in deterministic tick order.
        (void)tick;
    }

    using PacketHandler = void (ClientSession::*)(const Packet&);

    void SetState(SessionState state)
    {
        m_state = state;

        switch (state) {
        case SessionState::AwaitingWorld:
            m_packetHandler = &ClientSession::ProcessAwaitingWorld;
            break;
        case SessionState::WorldReady:
            m_packetHandler = &ClientSession::ProcessWorldReady;
            break;
        case SessionState::AwaitingConnection:
        case SessionState::Disconnected:
            m_packetHandler = &ClientSession::ProcessInactive;
            break;
        }
    }

    void ProcessPacket(const Packet& request)
    {
        (this->*m_packetHandler)(request);
    }

    void ProcessInactive(const Packet& request)
    {
        SendError(request.request_id(), islewright::protocol::ErrorResponse::INVALID_REQUEST,
                  "Session is not active");
    }

    void ProcessAwaitingWorld(const Packet& request)
    {
        if (!request.has_create_world_request()) {
            SendError(request.request_id(), islewright::protocol::ErrorResponse::INVALID_REQUEST,
                      "CreateWorldRequest is required before using world services");
            return;
        }

        const auto& createRequest = request.create_world_request();
        if (!createRequest.has_seed()) {
            SendError(request.request_id(), islewright::protocol::ErrorResponse::INVALID_REQUEST,
                      "CreateWorldRequest requires a seed");
            return;
        }

        const std::uint64_t seed = createRequest.seed();
        m_world = std::make_unique<World>(seed);

        Packet response;
        response.set_protocol_version(ProtocolVersion);
        response.set_request_id(request.request_id());
        response.mutable_create_world_response()->set_seed(m_world->Seed());

        if (!SendPacket(response)) {
            m_world.reset();
            SendError(request.request_id(), islewright::protocol::ErrorResponse::INTERNAL_ERROR,
                      "Failed to send CreateWorldResponse");
            return;
        }

        SetState(SessionState::WorldReady);
        std::cout << "[WORLD] Created world with seed " << m_world->Seed() << '\n';
    }

    void ProcessWorldReady(const Packet& request)
    {
        if (request.has_create_world_request()) {
            SendError(request.request_id(), islewright::protocol::ErrorResponse::INVALID_REQUEST,
                      "World is already created for this session");
            return;
        }

        SendError(request.request_id(), islewright::protocol::ErrorResponse::INVALID_REQUEST,
                  "Unsupported service for WorldReady session");
    }

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
        response.set_protocol_version(ProtocolVersion);
        response.set_request_id(requestId);
        auto* error = response.mutable_error_response();
        error->set_code(code);
        error->set_message(message);

        if (!SendPacket(response)) {
            std::cerr << "[ERROR] Failed to send ErrorResponse\n";
        }
    }

    std::unique_ptr<World> m_world;
    SessionState m_state = SessionState::AwaitingConnection;
    PacketHandler m_packetHandler = &ClientSession::ProcessInactive;
    GameLoop m_gameLoop;
    std::mutex m_packetMutex;
    std::queue<Packet> m_packets;
};

} // namespace islewright::clientsession

#endif // ISLEWRIGHT_CLIENTSESSION_HPP
