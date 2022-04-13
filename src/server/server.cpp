/// @file server.cpp
#include "server/server.h"
#include "json_wrapper.hpp"

#include <cstring>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
#include <algorithm>
#include <errno.h>

/**
 * @brief Initialize servet TCP and UDP sockets, start multicasting map data.
 * 
 * @param[in] ttl_number
 * @return 0 in success, one of ::NetworkCode or ::MulticastCode enum code in error
 */
int Server::init(char ttl_number)
{
	can_notify_map = true;
	thread_id = 0;
	int ret = 0;
	if((ret = initServer()) < 0){
		return ret;
	}

	if((ret = initMulticast(ttl_number)) < 0)
	{
		return ret;
	}
	createNotifyMapThread();
	return 0;
}

/**
 * @brief Create TCP listen socket.
 * 
 * @return 0 in success, one of ::NetworkCode enum code in error
 */
int Server::initServer()
{
	listen_sock = socket(AF_INET, SOCK_STREAM, 0); 
	if (listen_sock < 0)
	{
		return NetworkCode::SOCKET_NOT_CREATED;
	}

	sockaddr_in listen_sock_addr;
	memset(&listen_sock_addr, 0, sizeof(listen_sock_addr));
	listen_sock_addr.sin_family = AF_INET;
	listen_sock_addr.sin_port = htons(TCP_PORT);
	inet_pton(AF_INET, ANY_IP, &listen_sock_addr.sin_addr);

	if (bind(listen_sock, (sockaddr *)&listen_sock_addr,
			 sizeof(listen_sock_addr)) == -1)
	{
		return NetworkCode::BIND_ERROR;
	}

	if (listen(listen_sock, 1) < 0)
	{
		return NetworkCode::LISTEN_ERROR;
	}
	return NetworkCode::SUCCESS;
}

/**
 * @brief Create UDP multicast socket. 
 * 
 * @param[in] ttl_number
 * @return 0 in success, one of ::MulticastCode enum code in error
 */
int Server::initMulticast(char ttl_number)
{
	multicast_sock = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&multicast_addr, 0, sizeof(multicast_addr));
    multicast_addr.sin_family = AF_INET;
    multicast_addr.sin_port = htons(UDP_PORT);
    multicast_addr.sin_addr.s_addr = inet_addr(MULTICAST_IP);
	
    ip_mreq multicast;	
    multicast.imr_interface.s_addr = inet_addr(ANY_IP);
    multicast.imr_multiaddr.s_addr = inet_addr(MULTICAST_IP);
    u_char loop = 1;
	if(setsockopt(multicast_sock, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop)) != 0)
	{
		int err = errno;
		fprintf(stderr,"[SERVER] multicast_init1: LOOP, errno=%i, str - %s\n", err, strerror(err));
		return MulticastCode::LOOP_SETUP_ERROR;
	}
	if(setsockopt(multicast_sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl_number, sizeof(ttl_number)) != 0)
	{
		int err = errno;
		fprintf(stderr,"[SERVER] multicast_init2: TTL, errno=%i, str - %s\n", err, strerror(err));
		return MulticastCode::TTL_SETUP_ERROR;
	}
    
	if(setsockopt(multicast_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &multicast, sizeof(multicast)) != 0)
	{
		int err = errno;
		fprintf(stderr,"[SERVER] multicast_init3: MEMBERSHIP, errno=%i, str - %s\n", err, strerror(err));
		return MulticastCode::MEMBERSHIP_ADD_ERROR;
	}

	return NetworkCode::SUCCESS;
}

/**
 * @brief Destroy the Server object.
 * 
 * Stop multicasting map data, close TCP and UDP sockets.
 */
Server::~Server()
{	
	can_notify_map = false;
	if(notify_map_thread.joinable())
	{
		notify_map_thread.join();
	}

	if (shutdown(listen_sock, SHUT_RDWR) != 0)
	{
		fprintf(stderr,"shutdow socket error, sock_fd=%d\n", listen_sock);
	}
	close(listen_sock);
	close(multicast_sock);	
}

/**
 * @brief Accept new connection.
 * 
 * Try to accept new user during 5 seconds.
 * @param[in out] remote_sock_addr struct to store IP:PORT 
 * @return file descriptor in success; NetworkCode::TIME_OUT enum code or -errno if accept error
 */
int Server::acceptConnection(sockaddr_in &remote_sock_addr)
{
    fd_set readfds;
    timeval time_out;
	socklen_t length = sizeof(remote_sock_addr);
	int remote_sock;
	time_out.tv_sec = 5;
	time_out.tv_usec = 0;

	FD_ZERO(&readfds);
	FD_SET(listen_sock, &readfds);
	remote_sock = select((listen_sock + 1) , &readfds , NULL , NULL , &time_out);
	if(remote_sock > 0)
	{ 
		if ((remote_sock = accept(listen_sock, 
				(sockaddr *)&remote_sock_addr, &length)) < 0)
		{ 
			int err = errno;
			return -err; 
		}
		printf("new connection: fd=%d\n", remote_sock);
		return remote_sock;
	}
	return NetworkCode::TIME_OUT;
}

/// @brief Send json with map data.
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

/// @brief Create thread to multicast data.
void Server::createNotifyMapThread()
{
	notify_map_thread = std::thread([&]()
	{
		while (can_notify_map)
		{	
            std::this_thread::sleep_for(NOTIFYING_DELAY);
			sendMap();
		}
	});
}

/**
 * @brief Append new connection to vector and start job.
 * 
 * @param[in] remote_sock socket file descriptor
 * @param[in] remote_sock_addr struct with IP:PORT
 */
void Server::startUserThread(int remote_sock, sockaddr_in &remote_sock_addr)
{
	clients.push_back(new RemoteClientManager 
			(remote_sock, &field , thread_id++, remote_sock_addr));
	clients.back()->run();
}
