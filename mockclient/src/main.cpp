#include "networkmanager.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

int main()
{
    islewright::networkmanager::NetworkManager networkManager;

    if (!networkManager.Connect()) {
        std::cerr << "[ERROR] Failed to connect to server\n";
        return EXIT_FAILURE;
    }

    networkManager.StartNetworking();

    std::cout << "Enter messages. Type 'quit' to exit.\n";

    std::string message;

    while (std::getline(std::cin, message)) {
        if (message == "quit") {
            break;
        }

        if (!networkManager.Send(message.data(), static_cast<int>(message.size()))) {
            std::cerr << "[ERROR] Failed to send message\n";
            break;
        }
    }

    networkManager.EndNetworking();

    return EXIT_SUCCESS;
}