#include "networkmanager.hpp"
#include "islewright/common/version.hpp"

#include <cstdlib>
#include <charconv>
#include <cstdint>
#include <iostream>
#include <string>

int main()
{
    std::cout << islewright::common::project_name() << " mockclient " << islewright::common::project_version() << '\n';

    islewright::networkmanager::NetworkManager networkManager;

    if (!networkManager.Connect()) {
        std::cerr << "[ERROR] Failed to connect to server\n";
        return EXIT_FAILURE;
    }

    networkManager.StartNetworking();

    std::cout << "Enter a world seed. Type 'quit' to exit.\n";

    std::string message;

    while (std::getline(std::cin, message)) {   
        if (message == "quit") {
            break;
        }

        std::uint64_t seed = 0;
        const auto [end, error] =
            std::from_chars(message.data(), message.data() + message.size(), seed);
        if (error != std::errc{} || end != message.data() + message.size()) {
            std::cerr << "[ERROR] Seed must be an unsigned 64-bit integer\n";
            continue;
        }

        if (!networkManager.RequestWorld(seed)) {
            std::cerr << "[ERROR] Failed to send CreateWorldRequest\n";
            break;
        }
    }

    networkManager.EndNetworking();

    return EXIT_SUCCESS;
}
