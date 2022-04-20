/// @file remote_client_manager.cpp
#include "server/remote_client_manager.h"

#include <unistd.h>
#include <string.h>

/**
 * @brief Construct a new RemoteClientManager object that responce for an user connection.
 * 
 * @param[in] remote_sock file descriptor
 * @param[in] field_r reference to Field object 
 * @param[in] thread_id this object id
 * @param[in] addr struct with IP:PORT
 */
RemoteClientManager::RemoteClientManager(int remote_sock,
        Field* field_r, int thread_id, sockaddr_in& addr) 
        : remote_socket(remote_sock), field(field_r), thread_id(thread_id)
{
    id = addNewUserToField(addr);
}

/**
 * @brief Close TCP socket
 * 
 * @param[in] sock file descriptor
 */
void RemoteClientManager::closeConnection(int sock)
{
    if (shutdown(sock, SHUT_RDWR) != 0)
    {
        int error = errno;
        fprintf(stderr,"error closing socket: strerror=%d: \
            %s thread_id=%d\n", error, strerror(error), thread_id);
    }
    if(close(sock) != 0 )
	{
		int err = errno;
		fprintf(stderr,"close fd error, remote_sock_fd=%d, errno=%i, \
			strerror - %s\n", sock, err, strerror(err));
	}
    printf("closed connection: sock_fd=%d \n", sock);
}

/**
 * @brief Add new user to global Field
 * 
 * @param[in] remote_sock_addr struct with IP:PORT
 * @return id number
 */
int RemoteClientManager::addNewUserToField(sockaddr_in &remote_sock_addr)
{
    char str[INET_ADDRSTRLEN] = {0};
    inet_ntop(AF_INET, &(remote_sock_addr.sin_addr), str, INET_ADDRSTRLEN);

    std::string ip = str;
    ip.append(":");
    ip.append(std::to_string(ntohs(remote_sock_addr.sin_port)));
    id = field->createUser(std::string(ip));

    send(remote_socket, &id, sizeof(id), 0);
    return id;
}

/**
 * @brief Receive char from client
 * 
 * @return char that belong to ::CommandKeys enum.
 */
char RemoteClientManager::recvMoveDirection()
{
    char command_buffer;
    int ret = recv(remote_socket,
             &command_buffer, sizeof(command_buffer), 0);

    int error_code = 0;
    socklen_t error_code_size = sizeof(error_code);
    int check = getsockopt(remote_socket, SOL_SOCKET, 
            SO_ERROR, &error_code, &error_code_size);

    printf("thread_id=%d\t ret=%d\t buff_char=%c\n", 
            thread_id, ret, command_buffer);

    if(check != 0 || ret == -1 || (ret == 0 && command_buffer == '\0'))
    {
        printf("%c, check=%i\n", CommandKeys::EXIT, check); 
        return CommandKeys::EXIT;
    }
    command_buffer = std::tolower(command_buffer);
    return command_buffer;
}

/**
 * @brief Move user at one position: left, right, up or down.
 * 
 * @param[in] move_direction char with move direction that belong to ::CommandKeys enum
 * @param[in out] to_move reference to User struct
 */
void RemoteClientManager::move(char move_direction, User &to_move)
{
    switch (move_direction)
    {
    case CommandKeys::DOWN:
        to_move.coords.y++;
        to_move.coords.y = (to_move.coords.y > (FIELD_HEIGHT-1)) ? 0 
                        : (to_move.coords.y < 0) ? (FIELD_HEIGHT-1)
                        : to_move.coords.y;
        return;

    case CommandKeys::LEFT:
        to_move.coords.x--;
        to_move.coords.x = (to_move.coords.x > (FIELD_WIDTH-1)) ? 0
                        : (to_move.coords.x < 0) ? (FIELD_WIDTH-1)
                        : to_move.coords.x;
        return;

    case CommandKeys::UP:
        to_move.coords.y--;
        to_move.coords.y = (to_move.coords.y > (FIELD_HEIGHT-1)) ? 0 
                        : (to_move.coords.y < 0) ? (FIELD_HEIGHT-1)
                        : to_move.coords.y;
        return;

    case CommandKeys::RIGHT:
        to_move.coords.x++;
        to_move.coords.x = (to_move.coords.x > (FIELD_WIDTH-1)) ? 0 
                        : (to_move.coords.x < 0) ? (FIELD_WIDTH-1)
                        : to_move.coords.x;
        return;
    }
}

/// @brief Start thread worker
void RemoteClientManager::run()
{
    manage_connection_thread = std::thread([&]()
    {
        this->manageConnection();
    });
}

/// @brief Stop thread worker
void RemoteClientManager::stop()
{
    if(manage_connection_thread.joinable())
    {
        manage_connection_thread.join();
    }
}

/// @brief Manage client connection 
void RemoteClientManager::manageConnection()
{
    User current_user;
    char command_buffer = '\0';
    current_user.uid = id;
    while (1)
    {
        command_buffer = recvMoveDirection();
        field->getUser(current_user.uid, current_user);
        move(command_buffer, current_user);
        if(command_buffer == CommandKeys::EXIT)
        {
            field->remove(current_user);
            break;
        }
        field->move(current_user);
    }
    closeConnection(remote_socket);
}
