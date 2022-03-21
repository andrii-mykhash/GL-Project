#ifndef GL_SERVER_H
#define GL_SERVER_H

#include "field.h"

#include <arpa/inet.h>
#include <thread>


class Server
{
private:
	const int SOCK_PORT = 8088;
	const char *SOCK_IP = "0.0.0.0";
    std::mutex map_mut;
	int listen_sock;

	int thread_id = 0;
	Field field;

public:
	Server();

	~Server();

	int acceptConnection(sockaddr_in& remote_sock_addr);
	
	void connectionManager(int remote_sock, const int thread_id);

	int createUser(int remote_sock, sockaddr_in& remote_sock_addr);

private:

	void sendMap(int remote_sock);

	int recvID(int remote_sock, const int thread_id);

	char recvMoveDirection(int remote_sock, const int thread_id);

	void move(char move_direction, User& to_move, int remote_sock);

	void closeRemoteConnection(int remote_sock, User to_remove);

};



#endif