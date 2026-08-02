#include "islewright/common/serializer.hpp"

#include "islewright.pb.h"

#include <cassert>
#include <string>

using islewright::common::ProtobufSerializer;
using islewright::protocol::Packet;

void RunSerializerTests()
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

    const char corrupt[] = {static_cast<char>(0xff)};
    assert(!ProtobufSerializer::Deserialize(corrupt, sizeof(corrupt), decoded));
    assert(!ProtobufSerializer::Deserialize(nullptr, 1, decoded));
}
