#include "islewright/common/version.hpp"
#include "networkmanager.hpp"

#include <charconv>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace {

void PrintHelp()
{
    std::cout << "\nAvailable commands:\n"
              << "  help                 Show this command guide\n"
              << "  world <seed>         Create a world with an unsigned 64-bit seed\n"
              << "  chunk <x> <y>        Request a chunk at the given integer coordinates\n"
              << "  origin               Request chunk (0, 0)\n"
              << "  quit                 Disconnect and exit\n"
              << "\nExamples:\n"
              << "  world 42\n"
              << "  chunk 1 0\n"
              << "  chunk -2 3\n\n";
}

std::vector<std::string> SplitCommand(const std::string& input)
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

} // namespace

int main()
{
    std::cout << islewright::common::project_name() << " mockclient "
              << islewright::common::project_version() << '\n';

    islewright::networkmanager::NetworkManager networkManager;

    if (!networkManager.Connect()) {
        std::cerr << "[ERROR] Failed to connect to server\n";
        return EXIT_FAILURE;
    }

    networkManager.StartNetworking();

    PrintHelp();

    std::string message;

    while (std::cout << "> " << std::flush, std::getline(std::cin, message)) {
        const std::vector<std::string> tokens = SplitCommand(message);
        if (tokens.empty()) {
            continue;
        }

        const std::string& command = tokens[0];

        if (command == "quit") {
            if (tokens.size() != 1) {
                std::cerr << "[ERROR] Usage: quit\n";
                continue;
            }
            break;
        }

        if (command == "help") {
            if (tokens.size() != 1) {
                std::cerr << "[ERROR] Usage: help\n";
                continue;
            }
            PrintHelp();
            continue;
        }

        if (command == "world") {
            std::uint64_t seed = 0;
            if (tokens.size() != 2 || !ParseNumber(tokens[1], seed)) {
                std::cerr << "[ERROR] Usage: world <unsigned 64-bit seed>\n";
                continue;
            }
            if (!networkManager.RequestWorld(seed)) {
                std::cerr << "[ERROR] Failed to send CreateWorldRequest\n";
                break;
            }
            continue;
        }

        if (command == "chunk") {
            int x = 0;
            int y = 0;
            if (tokens.size() != 3 || !ParseNumber(tokens[1], x) ||
                !ParseNumber(tokens[2], y)) {
                std::cerr << "[ERROR] Usage: chunk <integer x> <integer y>\n";
                continue;
            }
            if (!networkManager.RequestChunk(x, y)) {
                std::cerr << "[ERROR] Failed to send ChunkRequest\n";
                break;
            }
            continue;
        }

        if (command == "origin") {
            if (tokens.size() != 1) {
                std::cerr << "[ERROR] Usage: origin\n";
                continue;
            }
            if (!networkManager.RequestChunk(0, 0)) {
                std::cerr << "[ERROR] Failed to send ChunkRequest\n";
                break;
            }
            continue;
        }

        std::cerr << "[ERROR] Unknown command: " << command
                  << ". Type 'help' to see available commands.\n";
    }

    networkManager.EndNetworking();

    return EXIT_SUCCESS;
}
