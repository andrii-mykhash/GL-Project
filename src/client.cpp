#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <errno.h>

#include <map>
#include <iostream>
#include "../inc/user.hpp"
#include "../inc/json_wrapper.hpp"

const int PORT = 8088;

int main()
{
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

            ret = recv(sock, &buffer_size, sizeof(size_t), 0);

            cbor_data.resize(buffer_size);

            printf("ret=%d, buffer_size=%lu",
                   ret, buffer_size);
//, vector_size=%lu
            ret = recv(sock, &cbor_data[0], buffer_size, 0);
            users = json_cbor_to_map(cbor_data);
            for (auto it = users.cbegin(); it != users.cend(); ++it)
            {
                std::cout << it->second.coords.x << "x"
                          << it->second.coords.y << ":\t" << it->second.ip << "\n";
            }
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