#include "command.hpp"
#include "islewright/common/version.hpp"
#include "networkmanager.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

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
        const auto command = islewright::mockcommand::Parse(message);
        using CommandType = islewright::mockcommand::Type;

        if (command.type == CommandType::Empty) {
            continue;
        }
        if (command.type == CommandType::Invalid) {
            std::cerr << "[ERROR] " << command.error << '\n';
            continue;
        }
        if (command.type == CommandType::Quit) {
            break;
        }
        if (command.type == CommandType::Help) {
            PrintHelp();
            continue;
        }
        if (command.type == CommandType::World) {
            if (!networkManager.RequestWorld(command.seed)) {
                std::cerr << "[ERROR] Failed to complete CreateWorldRequest\n";
                break;
            }
            continue;
        }
        if (command.type == CommandType::Chunk) {
            if (!networkManager.RequestChunk(command.x, command.y)) {
                std::cerr << "[ERROR] Failed to complete ChunkRequest\n";
                break;
            }
            continue;
        }
        if (command.type == CommandType::Origin && !networkManager.RequestChunk(0, 0)) {
            std::cerr << "[ERROR] Failed to complete ChunkRequest\n";
            break;
        }
    }

    networkManager.EndNetworking();
    return EXIT_SUCCESS;
}
