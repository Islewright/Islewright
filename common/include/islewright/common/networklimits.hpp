#ifndef ISLEWRIGHT_COMMON_NETWORKLIMITS_HPP
#define ISLEWRIGHT_COMMON_NETWORKLIMITS_HPP

#include <cstdint>

namespace islewright::common::networklimits {

// Amount read from the socket at once. A complete frame may span multiple reads.
inline constexpr int RECEIVE_BUFFER_SIZE = 4 * 1024;

// Maximum accepted protobuf payload size, excluding the 4-byte length prefix.
inline constexpr std::uint32_t MAX_FRAME_SIZE = 1024 * 1024;

} // namespace islewright::common::networklimits

#endif // ISLEWRIGHT_COMMON_NETWORKLIMITS_HPP
