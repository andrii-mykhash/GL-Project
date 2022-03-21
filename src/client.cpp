#include <arpa/inet.h>
#include <unistd.h>

#include "../inc/client.h"
#include "../inc/json_wrapper.hpp"

UserClient::UserClient(std::string ip, const int port)
{
    sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        throw "socket creation error";
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0)
    {
        throw "invalid addr";
    }

    if (connect(sock, (sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        throw "connection failed";
    }
    recvID();
    initTTY();
}

UserClient::~UserClient()
{
    if (shutdown(sock, SHUT_RDWR) != 0)
    {
        perror("socket shutdown failed");
    }
    close(sock);
}

void UserClient::recvMap()
{
    cbor_data.clear();
    size_t buffer_size = 0;
    std::lock_guard<std::mutex> lock(map_mutex);
    ret = recv(sock, &buffer_size, sizeof(buffer_size), 0);
    if (ret != sizeof(buffer_size))
    {
        printf("error, ret-%d != %lu - sizeof(buffer_size)\n", ret, buffer_size);
    }
    cbor_data.resize(buffer_size);
    ret = recv(sock, cbor_data.data(), buffer_size, 0);
    if ((size_t)ret != buffer_size)
    {
        printf("error, ret-%d != %lu - map buffer_size", ret, buffer_size);
    }
    users = json_to_map(cbor_data);
}

bool UserClient::isMovableChar(char move_offset)
{
    return (move_offset == ComandKeys::W)
               ? false : (move_offset == ComandKeys::A)
               ? false : (move_offset == ComandKeys::S)
               ? false : (move_offset == ComandKeys::D)
               ? false : (move_offset == ComandKeys::Q)
               ? false : true;
}

void UserClient::sendMoveDirection(char move_offset)
{
    send(sock, &id, sizeof(id), 0);
    send(sock, &move_offset, sizeof(move_offset), 0);
}

void UserClient::recvID()
{
    ret = recv(sock, &id, sizeof(id), 0);
    if (ret != sizeof(id))
    {
        printf("error, ret-%d != 4 - sizeof(id)\n", ret);
    }
}

void UserClient::initTTY()
{//                        /dev/stdout    /dev/tty
    tty = std::make_unique<std::fstream>("/dev/stdout", tty->in | tty->out | tty->trunc);
    if (!tty->is_open())
    {
        throw "tty open error";
    }
}

void UserClient::draw()
{
    printAllUsers();
    drawField();
}

void UserClient::printAllUsers()
{
    *tty << "\033c";

    for (auto it = users.cbegin(); it != users.cend(); ++it)
    {
        if (it->first == id)
        {
            *tty << CYAN << it->second.coords.x << "x"
                 << it->second.coords.y << ":\t" << it->second.ip << "\n"
                 << RESET;
            continue;
        }
        *tty << it->first << ")\t" << it->second.coords.x << "x"
             << it->second.coords.y << ":\t" << it->second.ip << "\n";
    }
    tty->flush();
}

void UserClient::drawField()
{
    /*
        Output field that have got 80x24 dimention and contain all curent users
    */
    tty->flush();
    char paint[80][24] = {'\0'};
    for (auto it = users.cbegin(); it != users.cend(); ++it)
    {
        if (it->first == id)
        {
            paint[it->second.coords.x][it->second.coords.y] = '@';
            continue;
        }
        paint[it->second.coords.x][it->second.coords.y] = '*';
    }

    *tty << "+";
    for (int i = 0; i < 80; i++)
    {
        *tty << "-";
    }
    *tty << "+\n";

    for (int y = 0; y < 24; y++)
    {
        *tty << "|";
        for (int x = 0; x < 80; x++)
        {
            if (paint[x][y] == '\0')
            {
                *tty << " ";
            }
            else
            {
                if (paint[x][y] == '*')
                {
                    *tty << paint[x][y];
                }
                else if (paint[x][y] == '@')
                {
                    *tty << CYAN << paint[x][y] << RESET;
                }
            }
        }
        *tty << "|\n";
    }

    *tty << "+";
    for (int i = 0; i < 80; i++)
    {
        *tty << "-";
    }
    *tty << "+\n";
    tty->flush();
}
