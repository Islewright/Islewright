#include "gameinstance.hpp"

#include "islewright/common/constants.hpp"
#include "islewright/common/protocolversion.hpp"
#include "islewright/common/world.hpp"

#include <cassert>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <vector>

using islewright::common::CHUNK_HEIGHT;
using islewright::common::CHUNK_WIDTH;
using islewright::common::PROTOCOL_VERSION;
using islewright::gameinstance::GameInstance;
using islewright::protocol::ErrorResponse;
using islewright::protocol::Packet;

namespace {

Packet MakeChunkRequest(std::uint64_t requestId, int x, int y)
{
    Packet request;
    request.set_protocol_version(PROTOCOL_VERSION);
    request.set_request_id(requestId);
    auto* chunkRequest = request.mutable_chunk_request();
    chunkRequest->set_chunk_x(x);
    chunkRequest->set_chunk_y(y);
    return request;
}

} // namespace

int main()
{
    GameInstance game;
    std::mutex responseMutex;
    std::condition_variable responseCondition;
    std::vector<Packet> responses;

    game.SetResponseHandler([&](const Packet& response) {
        {
            std::lock_guard<std::mutex> lock(responseMutex);
            responses.push_back(response);
        }
        responseCondition.notify_one();
        return true;
    });
    game.Start();

    const auto waitForResponses = [&](std::size_t count) {
        std::unique_lock<std::mutex> lock(responseMutex);
        return responseCondition.wait_for(lock, std::chrono::seconds(2),
                                          [&] { return responses.size() >= count; });
    };

    game.Enqueue(MakeChunkRequest(1, 0, 0));
    assert(waitForResponses(1));
    {
        std::lock_guard<std::mutex> lock(responseMutex);
        assert(responses[0].request_id() == 1);
        assert(responses[0].has_error_response());
        assert(responses[0].error_response().code() == ErrorResponse::INVALID_REQUEST);
    }

    Packet createWorld;
    createWorld.set_protocol_version(PROTOCOL_VERSION);
    createWorld.set_request_id(2);
    createWorld.mutable_create_world_request()->set_seed(42);
    game.Enqueue(std::move(createWorld));
    assert(waitForResponses(2));
    {
        std::lock_guard<std::mutex> lock(responseMutex);
        assert(responses[1].request_id() == 2);
        assert(responses[1].has_create_world_response());
        assert(responses[1].create_world_response().seed() == 42);
    }

    game.Enqueue(MakeChunkRequest(3, -2, 3));
    assert(waitForResponses(3));
    {
        std::lock_guard<std::mutex> lock(responseMutex);
        const Packet& response = responses[2];
        assert(response.request_id() == 3);
        assert(response.has_chunk_response());
        assert(response.chunk_response().chunk_x() == -2);
        assert(response.chunk_response().chunk_y() == 3);
        assert(response.chunk_response().tile_ids().size() == CHUNK_WIDTH * CHUNK_HEIGHT);
        assert(response.chunk_response().biomes().size() == CHUNK_WIDTH * CHUNK_HEIGHT);

        islewright::common::World expectedWorld{42};
        const auto expectedEntity =
            expectedWorld.EnsureChunk(islewright::common::ChunkCoord{-2, 3});
        const auto& expectedChunk =
            expectedWorld.Registry().get<islewright::common::Chunk>(expectedEntity);

        for (std::size_t i = 0; i < expectedChunk.tiles.size(); ++i) {
            assert(static_cast<unsigned char>(response.chunk_response().tile_ids()[i]) ==
                   static_cast<unsigned char>(expectedChunk.tiles[i].id));
            assert(static_cast<unsigned char>(response.chunk_response().biomes()[i]) ==
                   static_cast<unsigned char>(expectedChunk.tiles[i].biome));
        }
    }

    game.Stop();
    game.SetResponseHandler({});
    return 0;
}
