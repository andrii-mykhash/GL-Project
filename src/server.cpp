#include "../inc/server.h"

#include <cstring>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
#include "../inc/json_wrapper.hpp"
#include <algorithm>

// Server::Server()
// {
// 	thread_id = 0;
// 	listen_sock = socket(AF_INET, SOCK_STREAM, 0); 
// 	if (listen_sock < 0)
// 	{
// 		int err = errno;
// 		// throw std::runtime_error()
// 		fprintf(stderr,"socket: strerror=%d: %s \n", err, strerror(err));
// 	}

// 	sockaddr_in listen_sock_addr;
// 	memset(&listen_sock_addr, 0, sizeof(listen_sock_addr));
// 	listen_sock_addr.sin_family = AF_INET;
// 	listen_sock_addr.sin_port = htons(SOCK_PORT);
// 	inet_pton(AF_INET, SOCK_IP, &listen_sock_addr.sin_addr);

// 	if (bind(listen_sock, (sockaddr *)&listen_sock_addr,
// 			 sizeof(listen_sock_addr)) == -1)
// 	{
// 		int err = errno;
// 		fprintf(stderr,"bind: strerror=%d: %s \n", err, strerror(err));
// 	}

// 	if (listen(listen_sock, 1) < 0)
// 	{
// 		int err = errno;
// 		fprintf(stderr,"listen: strerror=%d: %s \n", err, strerror(err));
// 	}
// }

void Server::setListenSock(int sock)
{
	this->listen_sock = sock;
}

void Server::setMulticastSock(int sock)
{
	this->multicast_sock = sock;
}

int Server::createSock(const char* ip, int port)
{
	int temp_sock = socket(AF_INET, SOCK_STREAM, 0); 
	if (temp_sock < 0)
	{
		int err = errno;
		// throw std::runtime_error()
		fprintf(stderr,"socket: strerror=%d: %s \n", err, strerror(err));
	}

	sockaddr_in sock_addr;
	memset(&sock_addr, 0, sizeof(sock_addr));
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(port);
	inet_pton(AF_INET, ip, &sock_addr.sin_addr);

	if (bind(temp_sock, (sockaddr *)&sock_addr,
			 sizeof(sock_addr)) == -1)
	{
		int err = errno;
		fprintf(stderr,"bind: strerror=%d: %s \n", err, strerror(err));
	}

	if (listen(temp_sock, 1) < 0)
	{
		int err = errno;
		fprintf(stderr,"listen: strerror=%d: %s \n", err, strerror(err));
	}
	return temp_sock;
}

Server::~Server()
{
	for (auto client: clients) {
		client->stop();
	}

	if(observer_thread.joinable())
	{
		observer_thread.join();
	}

	if (shutdown(listen_sock, SHUT_RDWR) != 0)
	{
		fprintf(stderr,"shutdow socket error, sock_fd=%d", listen_sock);
	}
	close(listen_sock);

	if (shutdown(multicast_sock, SHUT_RDWR) != 0)
	{
		fprintf(stderr,"shutdow socket error, sock_fd=%d", multicast_sock);
	}
	close(multicast_sock);
}

int Server::acceptConnection(int sock, sockaddr_in &remote_sock_addr)
{
	socklen_t length = sizeof(remote_sock_addr);
	int remote_sock;
	if ((remote_sock = accept(listen_sock, 
			(sockaddr *)&remote_sock_addr, &length)) < 0)
	{
		int err = errno;
		fprintf(stderr,"accept: strerror=%d: %s \n", err, strerror(err));
	}
	printf("new connection: fd=%d \n", remote_sock);
	return remote_sock;
}

void Server::acceptToMulticastSocket(int sock)
{
	std::lock_guard<std::mutex> lock(mult_mutex);
	remote_sockets.push_back(sock);
}

void Server::observeMap()
{
	observer_thread = std::thread([&]()
	{
		while (1)
		{	
            // usleep(300);
			sleep(1);
			//usleep(100);
			size_t map_buffer_size;
			std::map<int, User> to_send = field.getMap();

			std::vector<std::uint8_t> cbor_map = map_to_json(to_send);
			map_buffer_size = cbor_map.size();
			if(remote_sockets.size() == 0)
			{
				sleep(5);
				continue;
			}
			for(auto& sock_ : remote_sockets)
			{
				std::lock_guard<std::mutex> lock(mult_mutex);
				if(send(sock_, &map_buffer_size, sizeof(map_buffer_size), 0) < 0)
				{
					remote_sockets.erase( std::remove (
						remote_sockets.begin(), 
						remote_sockets.end(), sock_),remote_sockets.end());
					// field.
					continue;
				}
				send(sock_, &cbor_map.data()[0], map_buffer_size, 0);
			}
		}
	});
}


void Server::createTread(int remote_sock, int ml_sock, sockaddr_in &remote_sock_addr)
{
	clients.push_back(new RemoteClientManager 
			(remote_sock, ml_sock, &field , thread_id++, remote_sock_addr));
	clients.back()->run();
}
