#include "command.hpp"

#include <cassert>
#include <cstdint>
#include <limits>
#include <string>

using islewright::mockcommand::Parse;
using islewright::mockcommand::Type;

int main()
{
    assert(Parse("").type == Type::Empty);
    assert(Parse("   ").type == Type::Empty);
    assert(Parse("help").type == Type::Help);
    assert(Parse("origin").type == Type::Origin);
    assert(Parse("quit").type == Type::Quit);

    const auto world = Parse("world 18446744073709551615");
    assert(world.type == Type::World);
    assert(world.seed == std::numeric_limits<std::uint64_t>::max());

    const auto chunk = Parse("chunk -2 3");
    assert(chunk.type == Type::Chunk);
    assert(chunk.x == -2);
    assert(chunk.y == 3);

    assert(Parse("world").type == Type::Invalid);
    assert(Parse("world -1").type == Type::Invalid);
    assert(Parse("world 18446744073709551616").type == Type::Invalid);
    assert(Parse("chunk 1").type == Type::Invalid);
    assert(Parse("chunk 2147483648 0").type == Type::Invalid);
    assert(Parse("origin extra").type == Type::Invalid);
    assert(Parse("unknown").type == Type::Invalid);
    return 0;
}
