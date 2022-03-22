#include "../inc/server.h"

#include <cstring>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>

Server::Server()
{
	thread_id = 0;
	listen_sock = socket(AF_INET, SOCK_STREAM, 0); 
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
	for (auto client: clients) {
		client->stop();
	}

	if (shutdown(listen_sock, SHUT_RDWR) != 0)
	{
		printf("cannot shutdows socket sock=%d", listen_sock);
	}
	close(listen_sock);
}

int Server::acceptConnection(sockaddr_in &remote_sock_addr)
{
	socklen_t length = sizeof(remote_sock_addr);
	int remote_sock;
	if ((remote_sock = accept(listen_sock, 
			(sockaddr *)&remote_sock_addr, &length)) < 0)
	{
		int err = errno;
		printf("accept: strerror=%d: %s \n", err, strerror(err));
	}
	printf("new connection: fd=%d \n", remote_sock);
	return remote_sock;
}

void Server::createTread(int remote_sock, sockaddr_in &remote_sock_addr)
{
	clients.push_back(new RemoteClientManager 
			(remote_sock, &field , thread_id++, remote_sock_addr));
	clients.back()->run();
}
