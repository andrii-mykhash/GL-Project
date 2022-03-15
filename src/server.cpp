#include <arpa/inet.h> //inet_pton
#include <cstring>	   //memset
#include <errno.h>	   //strerror
#include <stdio.h>	   //printf
#include <sys/socket.h>
#include <unistd.h> //close
#include <stdlib.h> //exit
#include <fcntl.h>
#include <string>

#include "json_wrapper.hpp"
#include "inc/field.h"

const int SOCK_PORT = 8088;
const char *SOCK_IP = "0.0.0.0"; // localhost //8.8.8.8

int main()
{
	int sock = socket(AF_INET, SOCK_STREAM, 0); // SOCK_STREAM|SOCK_NONBLOCK
	if (sock < 0)
	{
		int err = errno;
		printf("socket: strerror=%d: %s \n", err, strerror(err));
		exit(-1);
	}

	sockaddr_in sa;
	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(SOCK_PORT);
	inet_pton(AF_INET, SOCK_IP, &sa.sin_addr);

	if (bind(sock, (sockaddr *)&sa, sizeof(sa)) == -1)
	{
		int err = errno;
		printf("bind: strerror=%d: %s \n", err, strerror(err));
		exit(-3);
	}

	if (listen(sock, 10) < 0)
	{
		int err = errno;
		printf("listen: strerror=%d: %s \n", err, strerror(err));
		exit(-2);
	}

	const int buf_len = 8;
	char buffer[buf_len];
	sockaddr_in saRemote;
	socklen_t length = sizeof(saRemote);
	memset(&sa, 0, sizeof(saRemote));
	int new_sock;
	if ((new_sock = accept(sock, (sockaddr *)&saRemote, &length)) < 0)
	{
		int err = errno;
		printf("accept: strerror=%d: %s \n", err, strerror(err));
		// exit(-3);
		// goto EXIT;
	}
	Field field;
	{
		char str[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(saRemote.sin_addr), str, INET_ADDRSTRLEN);
		field.createUser(std::string(str));
	}
	
	int ret;

	unsigned char coord_buffer[2] = {0};
	while (1)
	{
		ret = recvfrom(new_sock, buffer, buf_len, 0, (sockaddr *)&saRemote, &length);
		printf("ret=%d \n", ret);

		if (strcmp(buffer, "move\0") == 0 && ret == 8)
		{
			ret = recvfrom(new_sock, coord_buffer, 2 * sizeof(unsigned char), 0, (sockaddr *)&saRemote, &length);
			printf("ret=%d\n",ret);
			if(ret == 2)
			{
				printf("x=%d, y=%d\n", coord_buffer[0],coord_buffer[1] );
				// sbuf.coord.x = htonl(sbuf.coord.x); sbuf.coord.y = htonl(sbuf.coord.y);

				// coord_buffer[0] = htonl(coord_buffer[0]); 
				// coord_buffer[1] = htonl(coord_buffer[1]);

				sendto(new_sock, coord_buffer, 2 * sizeof(unsigned char), 0, (sockaddr *)&saRemote, length);
			}
		}
		if(strcmp(buffer, "q") == 0 || strcmp(buffer,"exit") == 0)
		{
			break;
		}
	} // while

	if (shutdown(sock, SHUT_RDWR) != 0)
	{
		exit(-4);
	}
	close(sock);

	exit(0);
}