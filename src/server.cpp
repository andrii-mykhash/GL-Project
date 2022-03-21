#include "../inc/server.h"

#include <cstring>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include "../inc/json_wrapper.hpp"
#include <atomic>

Server::Server()
{
	listen_sock = socket(AF_INET, SOCK_STREAM, 0); // SOCK_STREAM|SOCK_NONBLOCK
	if (listen_sock < 0)
	{
		int err = errno;
		printf("socket: strerror=%d: %s \n", err, strerror(err));
		exit(-1);
	}

	sockaddr_in listen_sock_addr;
	memset(&listen_sock_addr, 0, sizeof(listen_sock_addr));
	listen_sock_addr.sin_family = AF_INET;
	listen_sock_addr.sin_port = htons(SOCK_PORT);
	inet_pton(AF_INET, SOCK_IP, &listen_sock_addr.sin_addr);

	if (bind(listen_sock, (sockaddr *)&listen_sock_addr,
			sizeof(listen_sock_addr)) == -1)
	{
		int err = errno;
		printf("bind: strerror=%d: %s \n", err, strerror(err));
	}

	if (listen(listen_sock, 1) < 0)
	{
		int err = errno;
		printf("listen: strerror=%d: %s \n", err, strerror(err));
	}
}

Server::~Server()
{
	if (shutdown(listen_sock, SHUT_RDWR) != 0)
	{
		printf("cannot shutdows socket sock=%d", listen_sock);
	}
	close(listen_sock);
}

int Server::acceptConnection(sockaddr_in& remote_sock_addr)
{
	socklen_t length = sizeof(remote_sock_addr);
	int remote_sock;
	if ((remote_sock = accept(listen_sock, (sockaddr *)&remote_sock_addr,
							&length)) < 0)
	{
		int err = errno;
		printf("accept: strerror=%d: %s \n", err, strerror(err));
	}
	printf("new connection accepted: %d \n", remote_sock);
	return remote_sock;
}

int Server::createUser(int remote_sock, sockaddr_in& remote_sock_addr)
{
	char str[INET_ADDRSTRLEN] = {0};
	inet_ntop(AF_INET, &(remote_sock_addr.sin_addr), str, INET_ADDRSTRLEN);
	int id = field.createUser(std::string(str));
	send(remote_sock, &id, sizeof(id),0);
	return id;
}

void Server::sendMap(int remote_sock)
{
	size_t map_buffer_size;
	std::lock_guard<std::mutex> lock(map_mut);
	std::map<int, User> to_send = field.getMap();

	std::vector<std::uint8_t> cbor_map = map_to_json(to_send);
	map_buffer_size = cbor_map.size();

	send(remote_sock, &map_buffer_size, sizeof(map_buffer_size),0);
	send(remote_sock, &cbor_map.data()[0], map_buffer_size, 0);
}

int Server::recvID(int remote_sock, const int thread_id)
{
	int id;
	int ret = recv(remote_sock, &id, sizeof(id), 0);
	printf("ret=%d\tbuff_id=%d\tthread_id=%d\n", ret, id, thread_id);//, temp.uid, thread_id);
	return id;
}

char Server::recvMoveDirection(int remote_sock, const int thread_id)
{	
	char command_buffer;
	int ret = recv(remote_sock, &command_buffer, sizeof(command_buffer), 0);
	printf("ret=%d\tbuff_char=%c\tthread_id=%d\n", ret, command_buffer, thread_id);
	return command_buffer;
}

void Server::move(char move_direction, User& to_move, int sock_to_close)
{
	switch (move_direction)
	{
		case 'q':
			closeRemoteConnection(sock_to_close, to_move);
		return;

		case 's':
			to_move.coords.y++;
			to_move.coords.y = (to_move.coords.y > 23) ?
						0 : (to_move.coords.y < 0) ? 23 : to_move.coords.y;
		return;

		case 'a':
			to_move.coords.x--;
			to_move.coords.x = (to_move.coords.x > 79) ? 
					0 : (to_move.coords.x < 0) ? 79 : to_move.coords.x;
		return;

		case 'w':
			to_move.coords.y--;
			to_move.coords.y = (to_move.coords.y > 23) ? 
					0 : (to_move.coords.y < 0) ? 23 : to_move.coords.y;
		return;

		case 'd':
			to_move.coords.x++;
			to_move.coords.x = (to_move.coords.x > 79) ? 
					0 : (to_move.coords.x < 0) ? 79 : to_move.coords.x;

		return;
	}
}

void Server::closeRemoteConnection(int remote_sock, User to_remove)
{
	field.remove(to_remove);
	if(shutdown(remote_sock,SHUT_RDWR) != 0)
	{
		int error = errno;
		printf("closing socket: strerror=%d: %s thread_id=%d\n", error, strerror(error), thread_id);
	}	
	close(remote_sock);
	printf("closed connection: %d \n", remote_sock);
}

void Server::connectionManager(int remote_sock, const int thread_id)
{
	User temp;	
	char command_buffer;
	while(1)
	{
		sendMap(remote_sock);
		
		temp.uid = recvID(remote_sock, thread_id);		
		command_buffer = recvMoveDirection(remote_sock, thread_id);
		field.getUser(temp.uid, temp);
		move(command_buffer,temp,remote_sock);
		if(command_buffer == 'q')
		{
			break;
		}
		field.move(temp);
		command_buffer = '\0';
	}
}
