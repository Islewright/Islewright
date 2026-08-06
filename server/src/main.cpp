#include "clientsession.hpp"
#include "islewright/common/version.hpp"

#include <cstdlib>
#include <iostream>

int main()
{
    std::cout << islewright::common::project_name() << " server "
              << islewright::common::project_version() << '\n';
    using ClientSession = islewright::clientsession::ClientSession;

    ClientSession session;

    if (!session.Run()) {
        return EXIT_FAILURE;
    }

    std::cout << "Press Enter to stop server\n";
    std::cin.get();

    session.End();

    return EXIT_SUCCESS;
}
