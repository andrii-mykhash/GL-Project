#include "../inc/server.h"
#include <vector>
#include <functional>
#include <cstring>

int main()
{       
    sockaddr_in sock_addr;
	std::vector<std::thread> threads;
	Server s;
    int client_countrer = 0;
    int remote_sock;
    auto connnect_manager = std::bind(&Server::connectionManager,
            std::ref(s), std::placeholders::_1, std::placeholders::_2);
    while(1)
    {
        remote_sock = 0;
        memset(&sock_addr, 0, sizeof(sock_addr));
       	remote_sock = s.acceptConnection(sock_addr);
		s.createUser(remote_sock, sock_addr);
        threads.push_back(
                std::thread(connnect_manager, remote_sock, client_countrer++));
    }
    for (auto & t : threads)
	{
		t.join();
	}

}