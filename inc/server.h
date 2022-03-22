#ifndef GL_SERVER_H
#define GL_SERVER_H

#include "field.h"

#include <arpa/inet.h>
#include <thread>
#include "remote_client_manager.h"

class Server
{
public:
	Server();

	~Server();

	void createTread(int remote_sock, sockaddr_in &remote_sock_addr);

	int acceptConnection(sockaddr_in& remote_sock_addr);

private:
	const int SOCK_PORT = 8088;
	const char *SOCK_IP = "0.0.0.0";
	int listen_sock;
	std::vector<RemoteClientManager*> clients;
	int thread_id;
	Field field;

};

#endif