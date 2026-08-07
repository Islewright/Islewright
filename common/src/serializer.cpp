#include "islewright/common/serializer.hpp"

#include "islewright.pb.h"

#include <limits>

namespace islewright::common {

namespace {
constexpr std::size_t MAX_SERIALIZED_SIZE = 1024 * 1024;
}

bool ProtobufSerializer::Serialize(const protocol::Packet& packet, std::string& output)
{
    if (packet.ByteSizeLong() > MAX_SERIALIZED_SIZE) {
        return false;
    }
    return packet.SerializeToString(&output);
}

bool ProtobufSerializer::Deserialize(const void* data, std::size_t size, protocol::Packet& packet)
{
    if ((data == nullptr && size != 0) || size > MAX_SERIALIZED_SIZE ||
        size > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
        return false;
    }
    packet.Clear();
    return packet.ParseFromArray(data, static_cast<int>(size));
}

} // namespace islewright::common
