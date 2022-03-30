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
        while (std::cin >> s)
        {
            if (s == "exit" || s == "q")
            {
                std::cout << "\nexit command given on stdin\nclossing...";
                can_work = false;
                break;
            }		
        }
    }};
    t1.detach();

	Server server;
    sockaddr_in sock_addr;
    int remote_sock;
    bool flag = true;
    while(can_work)
    {
        memset(&sock_addr, 0, sizeof(sock_addr));
        remote_sock = server.acceptConnection(sock_addr);
        server.createTread(remote_sock, sock_addr);
        if(flag)
        {
            server.notifyMap();
            flag = false;
        }
    }
    return 0;
}