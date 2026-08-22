#include "islewright/common/serializer.hpp"

#include "islewright.pb.h"

#include <cassert>
#include <string>

using islewright::common::ProtobufSerializer;
using islewright::protocol::Packet;

void RunSerializerTests()
{
    {
        Packet request;
        request.set_protocol_version(1);
        request.set_request_id(42);
        request.mutable_create_world_request()->set_seed(1234);

        std::string serialized;
        assert(ProtobufSerializer::Serialize(request, serialized));
        assert(!serialized.empty());

        Packet decoded;
        assert(ProtobufSerializer::Deserialize(serialized.data(), serialized.size(), decoded));
        assert(decoded.protocol_version() == 1);
        assert(decoded.request_id() == 42);
        assert(decoded.has_create_world_request());
        assert(decoded.create_world_request().has_seed());
        assert(decoded.create_world_request().seed() == 1234);
    }

    {
        Packet request;
        request.set_protocol_version(1);
        request.set_request_id(43);
        auto* chunkRequest = request.mutable_chunk_request();
        chunkRequest->set_chunk_x(-2);
        chunkRequest->set_chunk_y(3);

        std::string serialized;
        assert(ProtobufSerializer::Serialize(request, serialized));

        Packet decoded;
        assert(ProtobufSerializer::Deserialize(serialized.data(), serialized.size(), decoded));
        assert(decoded.request_id() == 43);
        assert(decoded.has_chunk_request());
        assert(decoded.chunk_request().chunk_x() == -2);
        assert(decoded.chunk_request().chunk_y() == 3);
    }

    {
        Packet response;
        response.set_protocol_version(1);
        response.set_request_id(44);
        auto* chunkResponse = response.mutable_chunk_response();
        chunkResponse->set_chunk_x(-2);
        chunkResponse->set_chunk_y(3);
        chunkResponse->set_tile_ids(std::string{"\x01\x02\x03", 3});
        chunkResponse->set_biomes(std::string{"\x04\x05\x06", 3});

        std::string serialized;
        assert(ProtobufSerializer::Serialize(response, serialized));

        Packet decoded;
        assert(ProtobufSerializer::Deserialize(serialized.data(), serialized.size(), decoded));
        assert(decoded.request_id() == 44);
        assert(decoded.has_chunk_response());
        assert(decoded.chunk_response().chunk_x() == -2);
        assert(decoded.chunk_response().chunk_y() == 3);
        const std::string expectedTileIds{"\x01\x02\x03", 3};
        const std::string expectedBiomes{"\x04\x05\x06", 3};
        assert(decoded.chunk_response().tile_ids() == expectedTileIds);
        assert(decoded.chunk_response().biomes() == expectedBiomes);
    }

    const char corrupt[] = {static_cast<char>(0xff)};
    Packet decoded;
    assert(!ProtobufSerializer::Deserialize(corrupt, sizeof(corrupt), decoded));
    assert(!ProtobufSerializer::Deserialize(nullptr, 1, decoded));
}
