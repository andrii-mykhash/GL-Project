#ifndef GL_SERVER_H
#define GL_SERVER_H

#include "field.h"
#include "remote_client_manager.h"

#include <arpa/inet.h>
#include <thread>
#include <mutex>

class Server
{
public:
	Server();

	~Server();

	void createTread(int remote_sock, sockaddr_in &remote_sock_addr);

	int acceptConnection(sockaddr_in &remote_sock_addr);

	int createSock(const char* ip, int port);

	void sendMap();

	void notifyMap();
	
	void initMulticast();

private:
	const int TCP_PORT = 8088;
	const int UDP_PORT = 8182;
	const char *ANY_IP = "0.0.0.0";
	const char *MULTICAST_IP = "229.0.0.80";
	int multicast_sock;
	sockaddr_in multicast_addr;
	int listen_sock;

	std::thread observer_thread;
	std::vector<RemoteClientManager*> clients;
	int thread_id;
	Field field;
};

#endif