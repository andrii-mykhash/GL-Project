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
    
	int createUser(sockaddr_in &remote_sock_addr);

	bool isCorrectChar(char to_verify);

	char recvMoveDirection();

	void move(char move_direction, User &to_move);

	void manageConnection();

private:
	int remote_socket;
    std::thread t_mc;
	Field* field;
	int thread_id;
	int id;
};

#endif
