#include "server/remote_client_manager.h"

#include <unistd.h>
#include <string.h>

RemoteClientManager::RemoteClientManager(int remote_sock,
        Field* field_r, int thread_id, sockaddr_in& addr) 
        : remote_socket(remote_sock), field(field_r), thread_id(thread_id)
{
    
    id = createUser(addr);
}

void RemoteClientManager::closeConnection(int sock)
{
    if (shutdown(sock, SHUT_RDWR) != 0)
    {
        int error = errno;
        fprintf(stderr,"error closing socket: strerror=%d: \
            %s thread_id=%d\n", error, strerror(error), thread_id);
    }
    close(sock);
    printf("close connection: sock_fd=%d \n", sock);
}

int RemoteClientManager::createUser(sockaddr_in &remote_sock_addr)
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

bool RemoteClientManager::isCorrectChar(char to_verify)
{
    return (to_verify == ComandKeys::UP)
               ? false : (to_verify == ComandKeys::LEFT)
               ? false : (to_verify == ComandKeys::DOWN)
               ? false : (to_verify == ComandKeys::RIGHT)
               ? false : (to_verify == ComandKeys::EXIT)
               ? false : true;
}

char RemoteClientManager::recvMoveDirection()
{
    char command_buffer;
    int ret = recv(remote_socket,
             &command_buffer, sizeof(command_buffer), 0);
    int error_code = 0;
    socklen_t error_code_size = sizeof(error_code);;
    int check = getsockopt(remote_socket, SOL_SOCKET, SO_ERROR, &error_code, &error_code_size);
    printf("thread_id=%d\t\tret=%d\t\tbuff_char=%c\n", 
            thread_id, ret, command_buffer);
    if(check != 0 || ret == -1 || (ret == 0 && command_buffer == '\0'))
    {
        printf("%c, check=%i\n", ComandKeys::EXIT, check); 
        return ComandKeys::EXIT;
    }
    return command_buffer;
}

void RemoteClientManager::move(char move_direction, User &to_move)
{
    switch (move_direction)
    {
    case ComandKeys::EXIT:
        field->remove(to_move);
        return;

    case ComandKeys::DOWN:
        to_move.coords.y++;
        to_move.coords.y = (to_move.coords.y > (FIELD_HEIGHT-1)) ? 0 
                        : (to_move.coords.y < 0) ? (FIELD_HEIGHT-1)
                        : to_move.coords.y;
        return;

    case ComandKeys::LEFT:
        to_move.coords.x--;
        to_move.coords.x = (to_move.coords.x > (FIELD_WIDTH-1)) ? 0
                        : (to_move.coords.x < 0) ? (FIELD_WIDTH-1)
                        : to_move.coords.x;
        return;

    case ComandKeys::UP:
        to_move.coords.y--;
        to_move.coords.y = (to_move.coords.y > (FIELD_HEIGHT-1)) ? 0 
                        : (to_move.coords.y < 0) ? (FIELD_HEIGHT-1)
                        : to_move.coords.y;
        return;

    case ComandKeys::RIGHT:
        to_move.coords.x++;
        to_move.coords.x = (to_move.coords.x > (FIELD_WIDTH-1)) ? 0 
                        : (to_move.coords.x < 0) ? (FIELD_WIDTH-1)
                        : to_move.coords.x;

        return;
    }
}

void RemoteClientManager::run()
{
    t_mc = std::thread([&]()
    {
        this->manageConnection();
    });
}

void RemoteClientManager::stop()
{
    if(t_mc.joinable())
    {
        t_mc.join();
    }
}

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
        if(command_buffer == ComandKeys::EXIT)
        {
            break;
        }
        field->move(current_user);
    }
    closeConnection(remote_socket);
}
