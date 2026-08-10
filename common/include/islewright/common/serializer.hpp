#ifndef ISLEWRIGHT_COMMON_SERIALIZER_HPP
#define ISLEWRIGHT_COMMON_SERIALIZER_HPP

#include <cstddef>
#include <string>

namespace islewright::protocol {
class Packet;
}

namespace islewright::common {

class ProtobufSerializer
{
  public:
    ProtobufSerializer() = delete;

    static bool Serialize(const protocol::Packet& packet, std::string& output);
    static bool Deserialize(const void* data, std::size_t size, protocol::Packet& packet);
};

} // namespace islewright::common

#endif // ISLEWRIGHT_COMMON_SERIALIZER_HPP
