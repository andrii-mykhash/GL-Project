#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <errno.h>

#include <fstream>
#include <map>
// #include <iostream>
#include "../inc/user.hpp"
#include "../inc/json_wrapper.hpp"

const int PORT = 8088;

void draw(std::fstream *tty, std::map<int,User> users)
{
    /*
        Output field that have got 80x24 dimention and contain all curent users
    */

    tty->flush();
    char paint[80][24] = {'\0'};
    for (auto it = users.cbegin(); it != users.cend(); ++it)
    {
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
                *tty << "\033[" << paint[x][y];
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





int main()
{

    std::unique_ptr<std::fstream> tty = std::make_unique<std::fstream>("/dev/tty", tty->in | tty->out | tty->trunc);
    if(!tty->is_open())
    {
        std::cerr << "tty open error" << std::endl;
        exit(-1);
    }

//-----------------------connecting stage------------------------
    char buffer[8];
    char *ip_str = (char *)malloc(16);
    printf("Write IP to connect: ");
    scanf("%s", ip_str);

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

    // create containers
    unsigned char x_y_buff[2] = {0};
    int ret = 0;
    int x_cin, y_cin;

    std::map<int, User> users;
    std::vector<std::uint8_t> cbor_data;
    while (1)
    {
        memset(&buffer, 0, 8 * sizeof(char));
        scanf("%s", buffer);
        send(sock, buffer, 8 * sizeof(char), 0);

// exit
        if (strcmp(buffer, "q") == 0 || 
            strcmp(buffer, "exit") == 0)
        {
            break;
        }

//  getiing map
        if (strcmp(buffer, "map\0") == 0)
        {
            cbor_data.clear();
            size_t buffer_size = 0;
            ret = recv(sock, &buffer_size, sizeof(buffer_size), 0);
            cbor_data.resize(buffer_size);

            // printf("ret=%d, buffer_size=%lu\n", ret, buffer_size);

            ret = recv(sock, cbor_data.data(), buffer_size, 0);
            users = json_to_map(cbor_data);
            
            for (auto it = users.cbegin(); it != users.cend(); ++it)
            {
                *tty << it->second.coords.x << "x"
                        << it->second.coords.y << ":\t" << it->second.ip << "\n";
                // printf("x=%i, y=%i, ip=%s\n",it->second.coords.x,it->second.coords.y,it->second.ip.c_str());
            }
            *tty << "\033c";
            draw(tty.get(), users);
        }

// moving dot
        if (strcmp(buffer, "move\0") == 0)
        {
            scanf("%d", &x_cin);
            scanf("%d", &y_cin);

            x_y_buff[0] = static_cast<unsigned char>(x_cin);
            x_y_buff[1] = static_cast<unsigned char>(y_cin);

            printf("x=%d, y=%d\n", (int)x_y_buff[0], (int)x_y_buff[1]);

            send(sock, x_y_buff, 2 * sizeof(unsigned char), 0);

            memset(x_y_buff, 0, 2 * sizeof(unsigned char));
            recv(sock, x_y_buff, 2 * sizeof(unsigned char), 0);
            printf("rec_x=%d, rec_y=%d\n", (int)x_y_buff[0], (int)x_y_buff[1]);
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