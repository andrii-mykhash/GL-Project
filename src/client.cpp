#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring> //memset
#include <errno.h>

// #include <bo
const int PORT = 8088;

int main()
{
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

    unsigned char coords_buff[2] = {0};
    int x, y;

    while (1)
    {
        memset(&buffer, 0, 8 * sizeof(char));
        scanf("%s", buffer);
        send(sock, buffer, 8 * sizeof(char), 0);
        if (strcmp(buffer, "q") == 0 || strcmp(buffer,"exit") == 0)
        {
            break;
        }
        scanf("%d", &x);
        scanf("%d", &y);

        coords_buff[0] = static_cast<unsigned char>(x);
        coords_buff[1] = static_cast<unsigned char>(y);

        printf("x=%d, y=%d\n", (int)coords_buff[0], (int)coords_buff[1]);

        send(sock, coords_buff, 2 * sizeof(unsigned char), 0);

        memset(coords_buff, 0, 2 * sizeof(unsigned char));
        recv(sock, coords_buff, 2 * sizeof(unsigned char), 0);
        printf("rec_x=%d, rec_y=%d\n", (int)coords_buff[0], (int)coords_buff[1]);
    }

    // scanf("%s", buffer);
    // strcpy(buffer,"move\0");
    // send(sock,buffer,32*sizeof(char),0);

    // memset(&buffer, 0, 32*sizeof(char));
    // recv(sock,buffer,32*sizeof(char),0);
    // printf("recv=%s",buffer);

    if (shutdown(sock, SHUT_RDWR) != 0)
    {
        exit(-4);
    }
    close(sock);

    return 0;
}