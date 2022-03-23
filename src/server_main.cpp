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
    const char ip[] = "0.0.0.0";
	Server s;
    sockaddr_in sock_addr_m;
    sockaddr_in sock_addr_l;
    int remote_m_sock;
    int remote_l_sock;
    int listen_s = s.createSock(ip,8088);
    int multicast_s = s.createSock(ip,8089);

    s.setListenSock(listen_s);
    s.setMulticastSock(multicast_s);
    bool flag = true;
    while(can_work)
    {
        memset(&sock_addr_m, 0, sizeof(sock_addr_m));
        memset(&sock_addr_l, 0, sizeof(sock_addr_l));
        remote_l_sock = s.acceptConnection(listen_s, sock_addr_l);
        remote_m_sock = s.acceptConnection(multicast_s, sock_addr_m);
        s.acceptToMulticastSocket(remote_m_sock);
        s.createTread(remote_l_sock, remote_m_sock, sock_addr_l);
        if(flag)
        {
            s.observeMap();
            flag = false;
        }
    }

}