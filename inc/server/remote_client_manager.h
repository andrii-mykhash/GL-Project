#ifndef GL_RC_MANAGER_H
#define GL_RC_MANAGER_H

#include "field.h"

#include <arpa/inet.h>
#include <thread>

class RemoteClientManager
{
public:
	RemoteClientManager(int remote_sock, Field* field_r,
			int thread_id, sockaddr_in& addr);

    void run();

    void stop(); 

private:
    void closeConnection(int sock);
    
	int addNewUserToField(sockaddr_in &remote_sock_addr);

	char recvMoveDirection();

	void move(char move_direction, User &to_move);

	void manageConnection();

private:
	int remote_socket;
    std::thread manage_connection_thread;
	Field* field;
	int thread_id;
	int id;
};

#endif
