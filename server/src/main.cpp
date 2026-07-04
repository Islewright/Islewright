#include "islewright/common/version.hpp"
#include "network/IslewrighterServer.hpp"
#include <iostream>
#include <string>

#define PORT 9000

#define MAX_WORKER_COUNT 16

#define MAX_CLIENT 1024

int main()
{
	std::cout << islewright::common::project_name() << " server "
		<< islewright::common::project_version() << '\n';

	IslewrighterServer islewrighterServer;
	islewrighterServer.Init(PORT, MAX_WORKER_COUNT);
	islewrighterServer.BindAndListen();
	islewrighterServer.Run(MAX_CLIENT);

	// Blocking for main thread termination
	std::string input;
	while (std::cin >> input)
	{
		if (input == "quit")
		{
			std::cout << "Stopping server...\n";
			break;
		}

		std::cout << "Unknown command. Type 'quit' to terminate.\n";
	}

	islewrighterServer.End();

	
	std::cout << "Server Terminated. Type any word to quit\n";
	std::cin >> input;

	std::cin.get();
	return 0;
}
