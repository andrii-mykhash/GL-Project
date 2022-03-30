#include "server/server.h"
#include "json_wrapper.hpp"

#include <cstring>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
#include <algorithm>
#include <errno.h>

Server::Server()
{
	thread_id = 0;
	listen_sock = socket(AF_INET, SOCK_STREAM, 0); 
	if (listen_sock < 0)
	{
		int err = errno;
        std::string str = "Server() socket: strerror=";
        str.append(std::to_string(err)).append(": ").append(strerror(err));
        throw std::runtime_error(str);
	}

	sockaddr_in listen_sock_addr;
	memset(&listen_sock_addr, 0, sizeof(listen_sock_addr));
	listen_sock_addr.sin_family = AF_INET;
	listen_sock_addr.sin_port = htons(TCP_PORT);
	inet_pton(AF_INET, ANY_IP, &listen_sock_addr.sin_addr);

	if (bind(listen_sock, (sockaddr *)&listen_sock_addr,
			 sizeof(listen_sock_addr)) == -1)
	{
		int err = errno;
        std::string str = "Server() bind: strerror=";
        str.append(std::to_string(err)).append(": ").append(strerror(err));
        throw std::runtime_error(str);
	}

	if (listen(listen_sock, 1) < 0)
	{
		int err = errno;
        std::string str = "Server() listen: strerror=";
        str.append(std::to_string(err)).append(": ").append(strerror(err));
        throw std::runtime_error(str);
	}
	initMulticast();
}

void Server::initMulticast()
{
	multicast_sock = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&multicast_addr, 0, sizeof(multicast_addr));
    multicast_addr.sin_family = AF_INET;
    multicast_addr.sin_port = htons(UDP_PORT);
    multicast_addr.sin_addr.s_addr = inet_addr(MULTICAST_IP);
	
    char ttl = 1;
    ip_mreq multicast;	
    multicast.imr_interface.s_addr = inet_addr(ANY_IP);
    multicast.imr_multiaddr.s_addr = inet_addr(MULTICAST_IP);
    u_char loop = 1;
	if(setsockopt(multicast_sock, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop)) != 0)
	{
		int err = errno;
		fprintf(stderr,"multicast_init: LOOP, errno=%i, str - %s\n", err, strerror(err));
	}
	if(setsockopt(multicast_sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) != 0)
	{
		int err = errno;
		fprintf(stderr,"multicast_init: TTL, errno=%i, str - %s\n", err, strerror(err));
	}
    
	if(setsockopt(multicast_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &multicast, sizeof(multicast)) != 0)
	{
		int err = errno;
		fprintf(stderr,"multicast_init: MEMBERSHIP, errno=%i, str - %s\n", err, strerror(err));
	}
    
}

Server::~Server()
{
	if(observer_thread.joinable())
	{
		observer_thread.join();
	}

	if (shutdown(listen_sock, SHUT_RDWR) != 0)
	{
		fprintf(stderr,"shutdow socket error, sock_fd=%d\n", listen_sock);
	}
	close(listen_sock);

	if (shutdown(multicast_sock, SHUT_RDWR) != 0)
	{
		fprintf(stderr,"shutdow socket error, sock_fd=%d\n", multicast_sock);
	}
	close(multicast_sock);
}

int Server::acceptConnection(sockaddr_in &remote_sock_addr)
{
	socklen_t length = sizeof(remote_sock_addr);
	int remote_sock;
	if ((remote_sock = accept(listen_sock, 
			(sockaddr *)&remote_sock_addr, &length)) < 0)
	{
		int err = errno;
		fprintf(stderr,"accept: strerror=%d: %s \n", err, strerror(err));
	}
	printf("new connection: fd=%d\n", remote_sock);
	return remote_sock;
}

void Server::sendMap()
{
	int ret = 0;
	std::vector<std::uint8_t> cbor_map;
	std::map<int, User> to_send;
	size_t map_buffer_size = 0;
	
	to_send = field.getMap();
	cbor_map = map_to_json(to_send);
	map_buffer_size = cbor_map.size();

	ret = sendto(multicast_sock, &map_buffer_size, sizeof(map_buffer_size), 0,
			(sockaddr*) &multicast_addr, sizeof(multicast_addr));
	if (ret == -1){
		int err = errno;
		fprintf(stderr, "sendMap1: errno=%i, str: %s\n", err, strerror(err));
	}
	ret = sendto(multicast_sock, cbor_map.data(), map_buffer_size, 0,
			(sockaddr*) &multicast_addr, sizeof(multicast_addr));
	if (ret == -1){
		int err = errno;
		fprintf(stderr, "sendMap2: errno=%i, str: %s\n", err, strerror(err));
	}
}


void Server::notifyMap()
{
	observer_thread = std::thread([&]()
	{
		while (1)
		{	
            usleep(130000);
			sendMap();
		}
	});
}

void Server::createTread(int remote_sock, sockaddr_in &remote_sock_addr)
{
	clients.push_back(new RemoteClientManager 
			(remote_sock, &field , thread_id++, remote_sock_addr));
	clients.back()->run();
}
