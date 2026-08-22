#ifndef ISLEWRIGHT_NETWORKMANAGER_HPP
#define ISLEWRIGHT_NETWORKMANAGER_HPP

#include "islewright.pb.h"
#include "islewright/common/biome.hpp"
#include "islewright/common/constants.hpp"
#include "islewright/common/protocolversion.hpp"
#include "islewright/common/serializer.hpp"
#include "islewright/common/tile.hpp"
#include "tcpconnector.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

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
        const std::uint64_t requestId = m_nextRequestId++;
        Packet request;
        request.set_protocol_version(islewright::common::PROTOCOL_VERSION);
        request.set_request_id(requestId);
        request.mutable_create_world_request()->set_seed(seed);

        std::string serialized;
        if (!ProtobufSerializer::Serialize(request, serialized) || serialized.empty()) {
            return false;
        }
        if (!Send(serialized.data(), static_cast<int>(serialized.size()))) {
            return false;
        }
        return WaitForResponse(requestId);
    }

    bool RequestChunk(int x, int y)
    {
        const std::uint64_t requestId = m_nextRequestId++;
        Packet request;
        request.set_protocol_version(islewright::common::PROTOCOL_VERSION);
        request.set_request_id(requestId);
        auto* chunkRequest = request.mutable_chunk_request();
        chunkRequest->set_chunk_x(x);
        chunkRequest->set_chunk_y(y);

        std::string serialized;
        if (!ProtobufSerializer::Serialize(request, serialized) || serialized.empty()) {
            return false;
        }

        {
            std::lock_guard<std::mutex> lock(m_pendingChunkMutex);
            m_pendingChunks.emplace(requestId, std::pair{x, y});
        }

        if (!Send(serialized.data(), static_cast<int>(serialized.size()))) {
            std::lock_guard<std::mutex> lock(m_pendingChunkMutex);
            m_pendingChunks.erase(requestId);
            return false;
        }

        if (WaitForResponse(requestId)) {
            return true;
        }

        std::lock_guard<std::mutex> lock(m_pendingChunkMutex);
        m_pendingChunks.erase(requestId);
        return false;
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

        if (response.protocol_version() != islewright::common::PROTOCOL_VERSION) {
            std::cerr << "[ERROR] Unsupported protocol version: " << response.protocol_version()
                      << '\n';
            return;
        }

        if (response.has_create_world_response()) {
            std::cout << "[WORLD] Server created world with seed "
                      << response.create_world_response().seed() << " (request "
                      << response.request_id() << ")\n";
        } else if (response.has_chunk_response()) {
            HandleChunkResponse(response);
        } else if (response.has_error_response()) {
            std::cerr << "[SERVER ERROR] code=" << response.error_response().code()
                      << ", message=" << response.error_response().message() << '\n';
            std::lock_guard<std::mutex> lock(m_pendingChunkMutex);
            m_pendingChunks.erase(response.request_id());
        } else {
            std::cerr << "[ERROR] Unexpected response payload\n";
        }

        MarkResponseCompleted(response.request_id());
    }

    void OnDisconnect() override
    {
        std::cout << "[DISCONNECT] Server Disconnected\n";
    }

  private:
    bool WaitForResponse(std::uint64_t requestId)
    {
        std::unique_lock<std::mutex> lock(m_completedResponseMutex);
        const bool received = m_responseCondition.wait_for(lock, std::chrono::seconds(5), [&] {
            return m_completedResponses.contains(requestId);
        });

        if (!received) {
            std::cerr << "[ERROR] Timed out waiting for response to request " << requestId << '\n';
            return false;
        }

        m_completedResponses.erase(requestId);
        return true;
    }

    void MarkResponseCompleted(std::uint64_t requestId)
    {
        {
            std::lock_guard<std::mutex> lock(m_completedResponseMutex);
            m_completedResponses.insert(requestId);
        }
        m_responseCondition.notify_one();
    }

    void HandleChunkResponse(const Packet& response)
    {
        const auto& chunk = response.chunk_response();
        std::pair<int, int> requestedCoord;
        {
            std::lock_guard<std::mutex> lock(m_pendingChunkMutex);
            const auto it = m_pendingChunks.find(response.request_id());
            if (it == m_pendingChunks.end()) {
                std::cerr << "[ERROR] Unexpected ChunkResponse request id " << response.request_id()
                          << '\n';
                return;
            }
            requestedCoord = it->second;
            m_pendingChunks.erase(it);
        }

        if (chunk.chunk_x() != requestedCoord.first || chunk.chunk_y() != requestedCoord.second) {
            std::cerr << "[ERROR] ChunkResponse coordinate mismatch: requested ("
                      << requestedCoord.first << ", " << requestedCoord.second << "), received ("
                      << chunk.chunk_x() << ", " << chunk.chunk_y() << ")\n";
            return;
        }

        constexpr std::size_t EXPECTED_TILE_COUNT =
            static_cast<std::size_t>(islewright::common::CHUNK_WIDTH) *
            islewright::common::CHUNK_HEIGHT;

        if (chunk.tile_ids().size() != EXPECTED_TILE_COUNT ||
            chunk.biomes().size() != EXPECTED_TILE_COUNT) {
            std::cerr << "[ERROR] Invalid chunk data size for (" << chunk.chunk_x() << ", "
                      << chunk.chunk_y() << "): tile_ids=" << chunk.tile_ids().size()
                      << ", biomes=" << chunk.biomes().size() << '\n';
            return;
        }

        std::size_t emptyCount = 0;
        std::size_t grassCount = 0;
        std::size_t waterCount = 0;
        std::size_t stoneCount = 0;

        for (std::size_t i = 0; i < EXPECTED_TILE_COUNT; ++i) {
            const auto tileId = static_cast<std::uint8_t>(chunk.tile_ids()[i]);
            const auto biome = static_cast<std::uint8_t>(chunk.biomes()[i]);

            if (tileId > static_cast<std::uint8_t>(islewright::common::TileId::Stone)) {
                std::cerr << "[ERROR] Invalid tile id " << static_cast<unsigned int>(tileId)
                          << " at local index " << i << '\n';
                return;
            }
            if (biome > static_cast<std::uint8_t>(islewright::common::BiomeType::Mountain)) {
                std::cerr << "[ERROR] Invalid biome " << static_cast<unsigned int>(biome)
                          << " at local index " << i << '\n';
                return;
            }

            switch (static_cast<islewright::common::TileId>(tileId)) {
            case islewright::common::TileId::Empty:
                ++emptyCount;
                break;
            case islewright::common::TileId::Grass:
                ++grassCount;
                break;
            case islewright::common::TileId::Water:
                ++waterCount;
                break;
            case islewright::common::TileId::Stone:
                ++stoneCount;
                break;
            }
        }

        std::cout << "[CHUNK] Received chunk (" << chunk.chunk_x() << ", " << chunk.chunk_y()
                  << ") (request " << response.request_id() << ")\n"
                  << "[CHUNK] tiles=" << chunk.tile_ids().size()
                  << ", biomes=" << chunk.biomes().size() << '\n'
                  << "[CHUNK] empty=" << emptyCount << ", grass=" << grassCount
                  << ", water=" << waterCount << ", stone=" << stoneCount << '\n';
    }

    std::atomic_uint64_t m_nextRequestId = 1;

    std::mutex m_completedResponseMutex;

    std::condition_variable m_responseCondition;

    std::unordered_set<std::uint64_t> m_completedResponses;

    std::mutex m_pendingChunkMutex;

    std::unordered_map<std::uint64_t, std::pair<int, int>> m_pendingChunks;
};

} // namespace islewright::networkmanager

#endif // ISLEWRIGHT_NETWORKMANAGER_HPP
