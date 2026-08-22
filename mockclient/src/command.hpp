#ifndef ISLEWRIGHT_MOCKCLIENT_COMMAND_HPP
#define ISLEWRIGHT_MOCKCLIENT_COMMAND_HPP

#include <charconv>
#include <cstdint>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace islewright::mockcommand {

enum class Type
{
    Empty,
    Help,
    World,
    Chunk,
    Origin,
    Quit,
    Invalid
};

struct Command
{
    Type type = Type::Invalid;
    std::uint64_t seed = 0;
    int x = 0;
    int y = 0;
    std::string error;
};

namespace detail {

inline std::vector<std::string> Split(const std::string& input)
{
    std::istringstream stream(input);
    std::vector<std::string> tokens;
    std::string token;
    while (stream >> token) {
        tokens.push_back(std::move(token));
    }
    return tokens;
}

template <typename T> bool ParseNumber(const std::string& input, T& value)
{
    const auto [end, error] =
        std::from_chars(input.data(), input.data() + input.size(), value);
    return error == std::errc{} && end == input.data() + input.size();
}

} // namespace detail

inline Command Parse(const std::string& input)
{
    const auto tokens = detail::Split(input);
    if (tokens.empty()) {
        return {.type = Type::Empty};
    }

    const std::string& name = tokens[0];
    if (name == "help") {
        return tokens.size() == 1
                   ? Command{.type = Type::Help}
                   : Command{.type = Type::Invalid, .error = "Usage: help"};
    }
    if (name == "quit") {
        return tokens.size() == 1
                   ? Command{.type = Type::Quit}
                   : Command{.type = Type::Invalid, .error = "Usage: quit"};
    }
    if (name == "origin") {
        return tokens.size() == 1
                   ? Command{.type = Type::Origin}
                   : Command{.type = Type::Invalid, .error = "Usage: origin"};
    }
    if (name == "world") {
        Command command{.type = Type::World};
        if (tokens.size() != 2 || !detail::ParseNumber(tokens[1], command.seed)) {
            return {.type = Type::Invalid,
                    .error = "Usage: world <unsigned 64-bit seed>"};
        }
        return command;
    }
    if (name == "chunk") {
        Command command{.type = Type::Chunk};
        if (tokens.size() != 3 || !detail::ParseNumber(tokens[1], command.x) ||
            !detail::ParseNumber(tokens[2], command.y)) {
            return {.type = Type::Invalid,
                    .error = "Usage: chunk <integer x> <integer y>"};
        }
        return command;
    }

    return {.type = Type::Invalid,
            .error = "Unknown command: " + name + ". Type 'help' to see available commands."};
}

} // namespace islewright::mockcommand

#endif // ISLEWRIGHT_MOCKCLIENT_COMMAND_HPP
