#include "islewright/common/version.hpp"
#include <iostream>
#include "TcpClient.hpp"

#define PORT 9000

int main(int argc, char* argv[])
{
    // WSAStartup 생략 (프로그램 진입점 등에서 1회 수행 필요)
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return -1;

    TcpClient client;
    if (client.Init(PORT)) // 서버 포트 지정
    {
        if (client.Connect("127.0.0.1"))
        {
            std::string input;
            while (true)
            {
                std::getline(std::cin, input);
                if (input == "exit") break;

                client.SendMessage(input);
            }
            client.Stop();
        }
    }

    WSACleanup();
    return 0;
}
