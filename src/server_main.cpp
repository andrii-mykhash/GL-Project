#include "../inc/server.h"
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

	Server s;
    sockaddr_in sock_addr;
    int remote_sock;
   
    while(can_work)
    {
        memset(&sock_addr, 0, sizeof(sock_addr));
        remote_sock = s.acceptConnection(sock_addr);
        s.createTread(remote_sock,sock_addr);
    }
   

}