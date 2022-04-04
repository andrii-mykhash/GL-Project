#include "../inc/server/server.h"

#include <cstring>
#include <iostream>
#include <atomic>

std::atomic<bool> can_work = true;
int main()
{       
    std::thread t1{
    [&]() 
    {	
        std::string s;
        std::cout << "[Info] To stop server enter \'q\'\n";
        while (std::cin >> s)
        {
            if (s == "exit" || s == "q")
            {
                std::cout << "\nexit command given on stdin\nclossing...\n";
                can_work = false;
                break;
            }		
        }
    }};
    t1.detach();

	Server server;

    {
        int ret_code;
        if((ret_code = server.init((char)1)) != 0)
        {
            return ret_code;
        }
    }
    sockaddr_in sock_addr;
    int remote_sock;

    while(can_work)
    {
        memset(&sock_addr, 0, sizeof(sock_addr));
        remote_sock = server.acceptConnection(sock_addr);
        if(remote_sock < 0)
        {
            continue;
        }
        server.startUserThread(remote_sock, sock_addr);
    }
    return 0;
}