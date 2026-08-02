#include "clientsession.hpp"
#include "gameinstance.hpp"
#include "islewright/common/version.hpp"

#include <cstdlib>
#include <iostream>

int main()
{
    std::cout << islewright::common::project_name() << " server "
              << islewright::common::project_version() << '\n';
    using ClientSession = islewright::clientsession::ClientSession;
    using GameInstance = islewright::gameinstance::GameInstance;

    GameInstance gameInstance;
    ClientSession session(gameInstance);

    if (!session.Listen()) {
        return EXIT_FAILURE;
    }

    std::cout << "[LISTEN] Waiting for client\n";

    if (!session.Accept()) {
        return EXIT_FAILURE;
    }

    session.StartNetworking();
    gameInstance.Start();

    std::cout << "Press Enter to stop server\n";
    std::cin.get();

    session.EndNetworking();
    gameInstance.Stop();

    return EXIT_SUCCESS;
}
