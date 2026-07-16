#include "islewright/common/version.hpp"

#include "TcpClient.hpp"

#include <iostream>

#define PORT 9000

int main(int argc, char* argv[])
{
    TcpClient client;
    
    if (!client.Init(PORT)) {
        return -1;
    }
    
    if (!client.Connect("127.0.0.1")) {
        return -1;
    }
    
    std::string input;
    
    while (true) {
        std::getline(std::cin, input);
        
        if (input == "exit") {
            break;
        }

        client.SendMessage(input);
    }
    
    client.Stop();

    return 0;
}
