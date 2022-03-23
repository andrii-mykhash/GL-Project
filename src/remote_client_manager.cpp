#include "../inc/remote_client_manager.h"
#include <unistd.h>
#include <string.h>



RemoteClientManager::RemoteClientManager(int remote_sock, int ml_sock,
        Field* field_r, int thread_id, sockaddr_in& addr) 
        : remote_socket(remote_sock), field(field_r), thread_id(thread_id), ml_sock(ml_sock)
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
    int id = field->createUser(std::string(ip));
    send(remote_socket, &id, sizeof(id), 0);
    return id;
}

// void RemoteClientManager::sendMap()
// {
//     size_t map_buffer_size;
//     std::map<int, User> to_send = field->getMap();

//     std::vector<std::uint8_t> cbor_map = map_to_json(to_send);
//     map_buffer_size = cbor_map.size();

//     send(remote_socket, &map_buffer_size, sizeof(map_buffer_size), 0);
//     send(remote_socket, &cbor_map.data()[0], map_buffer_size, 0);
// }

char RemoteClientManager::recvMoveDirection()
{
    char command_buffer;
    int ret = recv(remote_socket,
             &command_buffer, sizeof(command_buffer), 0);
    printf("ret=%d\tbuff_char=%c\tthread_id=%d\n", 
            ret, command_buffer, thread_id);
    return command_buffer;
}

void RemoteClientManager::move(char move_direction, User &to_move)
{
    switch (move_direction)
    {
    case 'q':
        field->remove(to_move);
        return;

    case 's':
        to_move.coords.y++;
        to_move.coords.y = (to_move.coords.y > 23) ? 0 
                        : (to_move.coords.y < 0) ? 23
                        : to_move.coords.y;
        return;

    case 'a':
        to_move.coords.x--;
        to_move.coords.x = (to_move.coords.x > 79) ? 0
                        : (to_move.coords.x < 0) ? 79
                        : to_move.coords.x;
        return;

    case 'w':
        to_move.coords.y--;
        to_move.coords.y = (to_move.coords.y > 23) ? 0 
                        : (to_move.coords.y < 0) ? 23
                        : to_move.coords.y;
        return;

    case 'd':
        to_move.coords.x++;
        to_move.coords.x = (to_move.coords.x > 79) ? 0 
                        : (to_move.coords.x < 0) ? 79
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
    t_mc.join();
}

void RemoteClientManager::manageConnection()
{
    User current_user;
    char command_buffer;
    current_user.uid = id;
    while (1)
    {
        // sendMap();
        command_buffer = recvMoveDirection();
        field->getUser(current_user.uid, current_user);
        move(command_buffer, current_user);
        if (command_buffer == 'q')
        {
            break;
        }
        field->move(current_user);
        command_buffer = '\0';
    }
    closeConnection(remote_socket);
    closeConnection(ml_sock);
}
