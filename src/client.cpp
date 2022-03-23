#include <arpa/inet.h>
#include <unistd.h>

#include "../inc/client.h"
#include "../inc/json_wrapper.hpp"

Client::Client(std::string ip, const int port)
{
    server_sock = connectToServer(ip,8088);


    multicast_sock = connectToServer(ip,8089);
    ret = recv(server_sock, &id, sizeof(id), 0);
    if (ret != sizeof(id))
    {
        int err = errno;
        std::string str = "recv: id - ";
        str.append(std::to_string(ret))
            .append(" != 4 - sizeof(int)\n strerror= ").append(strerror(err));
        throw std::runtime_error(str);
    }
    initTTY();
    map_tread = std::thread([&]
    {
        while(move_char != 'q')
        {
            // usleep(300);
            sleep(1);
            recvMap();
            draw();
            *tty << "Move - w/a/s/d; Quit - q: ";
            tty->flush();
        }
    });
}

Client::~Client()
{
    if (shutdown(multicast_sock, SHUT_RDWR) != 0)
    {
        fprintf(stderr,"socket shutdown failed");
    }
    close(multicast_sock);
    if(map_tread.joinable())
    {
        map_tread.join();
    }
    if (shutdown(server_sock, SHUT_RDWR) != 0)
    {
        fprintf(stderr,"socket shutdown failed");
    }
    close(server_sock);
}

int Client::connectToServer(std::string ip, const int port)
{
    int sock;
    sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        int err = errno;
        std::string str = "socket: strerror= ";
        str.append(strerror(err));
        throw std::runtime_error(str);
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0)
    {
        int err = errno;
        std::string str = "inet_pton: invalid address, strerror= ";
        str.append(strerror(err));
        throw std::runtime_error(str);
    }

    if (connect(sock, (sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        int err = errno;
        std::string str = "connect: strerror= ";
        str.append(strerror(err));
        throw std::runtime_error(str);
    }
    return sock;
}



int Client::recvMap()
{
    std::lock_guard<std::mutex> lock(map_mutex);
    cbor_data.clear();
    size_t buffer_size = 0;
    ret = recv(multicast_sock, &buffer_size, sizeof(buffer_size), 0);

    cbor_data.resize(buffer_size);
    ret = recv(multicast_sock, cbor_data.data(), buffer_size, 0);
    if ((size_t)ret != buffer_size)
    {
        fprintf(stderr,"corrupted json length error: ret - %d != %lu - buffer_size", ret, buffer_size);
        return -1;
    }
    users = json_to_map(cbor_data);
    return 0;
}

bool Client::isMovableChar(char move_offset)
{
    return (move_offset == ComandKeys::W)
               ? false : (move_offset == ComandKeys::A)
               ? false : (move_offset == ComandKeys::S)
               ? false : (move_offset == ComandKeys::D)
               ? false : (move_offset == ComandKeys::Q)
               ? false : true;
}

void Client::sendMoveDirection(char move_offset)
{
    move_char = move_offset;
    send(server_sock, &move_offset, sizeof(move_offset), 0);
}

void Client::initTTY()
{//                        /dev/stdout    /dev/tty
    tty = std::make_unique<std::fstream>("/dev/stdout", tty->in | tty->out | tty->trunc);
    if (!tty->is_open())
    {
        throw std::runtime_error("tty open error");
    }
}

void Client::draw()
{
    *tty << "\033c";//[2J\033[;H";
    printAllUsers();
    drawField();
    tty->flush();
}

void Client::printAllUsers()
{
    for (auto it = users.cbegin(); it != users.cend(); ++it)
    {
        if (it->first == id)
        {
            *tty << it->first << ")\t" << CYAN << it->second.coords.x << "x"
                 << it->second.coords.y << ":\t" << it->second.ip << "\n"
                 << RESET;
            continue;
        }
        *tty << it->first << ")\t" << it->second.coords.x << "x"
             << it->second.coords.y << ":\t" << it->second.ip << "\n";
    }
}

void Client::drawField()
{
    /*
        Output field that have got 80x24 dimention and contain all curent users
    */
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
}
