#ifndef ISLEWRIGHT_COMMON_SERIALIZER_HPP
#define ISLEWRIGHT_COMMON_SERIALIZER_HPP

#include <optional>
#include <string>
#include <string_view>

namespace islewright::common {

class ProtobufSerializer
{
  public:
    ProtobufSerializer() = delete;

    static std::optional<std::string> Serialize();

    static bool Deserialize(const char* msg, const int len);
};

} // namespace islewright::common

#endif // ISLEWRIGHT_COMMON_SERIALIZER_HPP
