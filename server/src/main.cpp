#include "islewright/common/version.hpp"

#include "IslewrightServer.hpp"

#include <iostream>

#define PORT 9000

int main()
{
	std::cout << islewright::common::project_name() << " server "
		<< islewright::common::project_version() << '\n';

	IslewrightServer islewrightServer;
	islewrightServer.BindAndListen(PORT);
	islewrightServer.StartServer();
	
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

	islewrightServer.EndServer();

	std::cout << "Server Terminated. Type any word to quit\n";
	std::cin >> input;

	std::cin.get();

	return 0;
}
