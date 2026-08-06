#ifndef ISLEWRIGHT_GAMEINSTANCE_HPP
#define ISLEWRIGHT_GAMEINSTANCE_HPP

#include "gameloop.hpp"
#include "islewright/common/world.hpp"
#include "islewright.pb.h"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <utility>

namespace islewright::gameinstance {

using GameLoop = islewright::gameloop::GameLoop;
using World = islewright::common::World;
using Packet = islewright::protocol::Packet;

enum class GameState
{
    AwaitingWorld,
    WorldReady
};

class GameInstance
{
  public:
    using ResponseHandler = std::function<bool(const Packet&)>;

    static constexpr std::uint32_t ProtocolVersion = 1;

    GameInstance() = default;
    GameInstance(const GameInstance&) = delete;
    GameInstance& operator=(const GameInstance&) = delete;
    GameInstance(GameInstance&&) = delete;
    GameInstance& operator=(GameInstance&&) = delete;

    ~GameInstance()
    {
        m_gameLoop.Stop();
    }

    void Start()
    {
        m_gameLoop.Start([this](std::uint64_t tick) { Tick(tick); });
    }

    void Stop()
    {
        m_gameLoop.Stop();
    }

    void SetResponseHandler(ResponseHandler handler)
    {
        std::lock_guard<std::mutex> lock(m_responseMutex);
        m_responseHandler = std::move(handler);
    }

    void Enqueue(Packet packet)
    {
        std::lock_guard<std::mutex> lock(m_packetMutex);
        if(m_packets.size() >= MAX_PACKET_QUEUE_SIZE) {
            return;
        }
        
        m_packets.push(std::move(packet));
    }

  private:
    static constexpr std::size_t MAX_PACKET_QUEUE_SIZE = 32;

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
            ProcessPacket(packets.front());
            packets.pop();
        }
    }

    void ProcessPacket(const Packet& request)
    {
        if (request.protocol_version() != ProtocolVersion) {
            SendError(request.request_id(),
                      islewright::protocol::ErrorResponse::UNSUPPORTED_VERSION,
                      "Unsupported protocol version");
            return;
        }

        (this->*m_packetHandler)(request);
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

        m_world = std::make_unique<World>(createRequest.seed());

        Packet response;
        response.set_protocol_version(ProtocolVersion);
        response.set_request_id(request.request_id());
        response.mutable_create_world_response()->set_seed(m_world->Seed());

        if (!SendResponse(response)) {
            m_world.reset();
            SendError(request.request_id(), islewright::protocol::ErrorResponse::INTERNAL_ERROR,
                      "Failed to send CreateWorldResponse");
            return;
        }

        SetState(GameState::WorldReady);
    }

    void ProcessWorldReady(const Packet& request)
    {
        if (request.has_create_world_request()) {
            SendError(request.request_id(), islewright::protocol::ErrorResponse::INVALID_REQUEST,
                      "World is already created");
            return;
        }

        SendError(request.request_id(), islewright::protocol::ErrorResponse::INVALID_REQUEST,
                  "Unsupported service for WorldReady game");
    }

    using PacketHandler = void (GameInstance::*)(const Packet&);

    void SetState(GameState state)
    {
        m_state = state;

        switch (state) {
        case GameState::AwaitingWorld:
            m_packetHandler = &GameInstance::ProcessAwaitingWorld;
            break;
        case GameState::WorldReady:
            m_packetHandler = &GameInstance::ProcessWorldReady;
            break;
        }
    }

    void UpdateWorld(std::uint64_t tick)
    {
        if (!m_world) {
            return;
        }

        // World simulation systems will be updated here in deterministic tick order.
        (void)tick;
    }

    bool SendResponse(const Packet& packet)
    {
        ResponseHandler handler;
        {
            std::lock_guard<std::mutex> lock(m_responseMutex);
            handler = m_responseHandler;
        }
        return handler && handler(packet);
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
        SendResponse(response);
    }

    GameLoop m_gameLoop;
    std::unique_ptr<World> m_world;
    GameState m_state = GameState::AwaitingWorld;
    PacketHandler m_packetHandler = &GameInstance::ProcessAwaitingWorld;

    std::mutex m_packetMutex;
    std::queue<Packet> m_packets;

    std::mutex m_responseMutex;
    ResponseHandler m_responseHandler;
};

} // namespace islewright::gameinstance

#endif // ISLEWRIGHT_GAMEINSTANCE_HPP
