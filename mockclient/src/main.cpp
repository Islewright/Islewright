#include "islewright/common/version.hpp"
#include <iostream>
#include "TcpClient.hpp"

#define PORT 9000

int main(int argc, char* argv[])
{
    TcpClient client;
<<<<<<< HEAD
    if (!client.Init(PORT)) // 서버 포트 지정
    {
        return -1;
    }
    if (!client.Connect("127.0.0.1"))
    {
=======
    
    if (!client.Init(PORT)) {
        return -1;
    }
    
    if (!client.Connect("127.0.0.1")) {
>>>>>>> 7cbd1e5aaf3f0645bb4f289dcf3bb1ee3058c735
        return -1;
    }
    
    std::string input;
<<<<<<< HEAD
    while (true)
    {
        std::getline(std::cin, input);
        if (input == "exit") break;

        client.SendMessage(input);
    }
=======
    
    while (true) {
        std::getline(std::cin, input);
        
        if (input == "exit") {
            break;
        }

        client.SendMessage(input);
    }
    
>>>>>>> 7cbd1e5aaf3f0645bb4f289dcf3bb1ee3058c735
    client.Stop();

    return 0;
}
