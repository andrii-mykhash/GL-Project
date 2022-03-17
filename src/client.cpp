#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <errno.h>

#include <fstream>
#include <map>
#include <iostream>
#include "../inc/user.hpp"
#include "../inc/json_wrapper.hpp"

const int PORT = 8088;

enum ComandKeys
{
    A = 'a',
    D = 'd',
    Q = 'q',
    S = 's',
    W = 'w'
};

const std::string RED("\033[0;31m");
const std::string GREEN("\033[1;32m");
const std::string YELLOW("\033[1;33m");
const std::string CYAN("\033[0;36m");
const std::string MAGENTA("\033[0;35m");
const std::string RESET("\033[0m");

void draw(std::fstream *tty, std::map<int, User> users, int id);

int main()
{
    // if debug raise 'tty open error' ->  change to /dev/stdout
    std::unique_ptr<std::fstream> tty = std::make_unique<std::fstream>("/dev/tty", tty->in | tty->out | tty->trunc);
    if (!tty->is_open())
    {
        std::cerr << "tty open error" << std::endl;
        exit(-1);
    }

    //-----------------------connecting stage------------------------

    char *ip_str = (char *)malloc(16);
    *tty << "Write IP to connect: ";
    tty->flush();
    std::cin >> ip_str;

    int sock = 0;
    sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket creation error");
        exit(-1);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, ip_str, &serv_addr.sin_addr) <= 0)
    {
        perror("invalid addr");
        exit(-2);
    }

    if (connect(sock, (sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("connection failed");
        exit(-3);
    }

    //-----------------------sharing data stage------------------------

    unsigned char move_offset = '\0';
    // unsigned char x_y_buff[2] = {0};
    int ret = 0;
    // int x_cin, y_cin;
    // char buffer[8];
    User currend_user;
    int id;
    std::map<int, User> users;
    std::vector<std::uint8_t> cbor_data;
    size_t buffer_size;
    bool is_command_char;
    ret = recv(sock, &id, sizeof(id), 0);
    // recieve id
    if (ret != sizeof(id))
    {
        printf("error, ret-%d != 4 - sizeof(id)\n", ret);
    }

    while (1)
    {
        // recieve and get map from json
        cbor_data.clear();
        buffer_size = 0;
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

        // draw field and show all users
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
        draw(tty.get(), users, id);

        // get move direction or quit
        *tty << "Move - w/a/s/d; Quit - q: ";
        tty->flush();

        do
        {
            std::cin >> move_offset;
            is_command_char = (move_offset == ComandKeys::W)
                                  ? false
                              : (move_offset == ComandKeys::A)
                                  ? false
                              : (move_offset == ComandKeys::S)
                                  ? false
                              : (move_offset == ComandKeys::D)
                                  ? false
                              : (move_offset == ComandKeys::Q)
                                  ? false
                                  : true;
            // printf("\n");
        } while (is_command_char);

        send(sock, &id, sizeof(id), 0);
        // send move direction
        send(sock, &move_offset, sizeof(move_offset), 0);
        if (move_offset == 'q')
        {
            break;
        }
    }

    //-----------------------cleaning stage------------------------

    if (shutdown(sock, SHUT_RDWR) != 0)
    {
        exit(-4);
    }
    close(sock);

    return 0;
}

void draw(std::fstream *tty, std::map<int, User> users, int id)
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

    // Draw grid
    // +----+
    // |    |
    // |    |
    // +----+

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
