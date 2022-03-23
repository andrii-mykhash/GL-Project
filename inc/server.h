#ifndef GL_SERVER_H
#define GL_SERVER_H

#include "field.h"

#include <arpa/inet.h>
#include <thread>
#include "remote_client_manager.h"
#include <mutex>

class Server
{
public:
	// Server();

	~Server();

	void createTread(int remote_sock,  int ml_sock, sockaddr_in &remote_sock_addr);

	int acceptConnection(int sock, sockaddr_in &remote_sock_addr);

	int createSock(const char* ip, int port);

	void setListenSock(int sock);

	void setMulticastSock(int sock);

	void acceptToMulticastSocket(int sock);

	void observeMap();

private:
	const int SOCK_PORT = 8088;
	const char *SOCK_IP = "0.0.0.0";
	int listen_sock;
	std::mutex mult_mutex;
	std::thread observer_thread;
	std::vector<RemoteClientManager*> clients;
	std::vector<int> remote_sockets;
	int thread_id;
	Field field;
	int multicast_sock;

};

#endif