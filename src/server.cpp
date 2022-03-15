#include <arpa/inet.h>
#include <cstring>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string>

#include "json_wrapper.hpp"
#include "../inc/field.h"

const int SOCK_PORT = 8088;
const char *SOCK_IP = "0.0.0.0";

int main()
{
	//---------------------creating lisen socket stage-----------------------------
	int listen_sock = socket(AF_INET, SOCK_STREAM, 0); // SOCK_STREAM|SOCK_NONBLOCK
	if (listen_sock < 0)
	{
		int err = errno;
		printf("socket: strerror=%d: %s \n", err, strerror(err));
		exit(-1);
	}

	sockaddr_in listen_sock_addr;
	memset(&listen_sock_addr, 0, sizeof(listen_sock_addr));
	listen_sock_addr.sin_family = AF_INET;
	listen_sock_addr.sin_port = htons(SOCK_PORT);
	inet_pton(AF_INET, SOCK_IP, &listen_sock_addr.sin_addr);

	if (bind(listen_sock, (sockaddr *)&listen_sock_addr,
			 sizeof(listen_sock_addr)) == -1)
	{
		int err = errno;
		printf("bind: strerror=%d: %s \n", err, strerror(err));
		exit(-3);
	}

	if (listen(listen_sock, 10) < 0)
	{
		int err = errno;
		printf("listen: strerror=%d: %s \n", err, strerror(err));
		exit(-2);
	}

	//---------------------accepting connection socket stage-----------------------------

	int remote_sock;
	sockaddr_in remote_sock_addr;
	memset(&listen_sock_addr, 0, sizeof(remote_sock_addr));
	socklen_t length = sizeof(remote_sock_addr);

	if ((remote_sock = accept(listen_sock, (sockaddr *)&remote_sock_addr,
							  &length)) < 0)
	{
		int err = errno;
		printf("accept: strerror=%d: %s \n", err, strerror(err));
		// exit(-3);
		// goto EXIT;
	}

	const int buf_len = 8;
	char command_buffer[buf_len];

	Field field;
	{
		// creating new user
		char str[INET_ADDRSTRLEN] = {0};
		inet_ntop(AF_INET, &(remote_sock_addr.sin_addr), str, INET_ADDRSTRLEN);
		field.createUser(std::string(str));//[0], str[INET_ADDRSTRLEN-1]));
		field.createUser(std::string("11.22.33.44"));//[0], str[INET_ADDRSTRLEN-1]));
	}

	int ret;
	std::map<int, User> to_send = field.getMap();
	unsigned char coord_buffer[2] = {0};
	while (1)
	{
		ret = recvfrom(remote_sock, command_buffer, buf_len, 0,
					   (sockaddr *)&remote_sock_addr, &length);
		printf("ret=%d \n", ret);

		if (strcmp(command_buffer, "move\0") == 0 && ret == 8)
		{
			ret = recvfrom(remote_sock, coord_buffer, 2 * sizeof(unsigned char),
						   0, (sockaddr *)&remote_sock_addr, &length);
			printf("ret=%d\n", ret);
			if (ret == 2)
			{
				printf("x=%d, y=%d\n", coord_buffer[0], coord_buffer[1]);

				sendto(remote_sock, coord_buffer, 2 * sizeof(unsigned char),
					   0, (sockaddr *)&remote_sock_addr, length);
			}
		}
		if (strcmp(command_buffer, "map\0") == 0 && ret == 8)
		{
			std::vector<std::uint8_t> cbor_map = map_to_cbor_json(to_send);
			size_t buffer_size = cbor_map.size();
			// send future buffer size

			sendto(remote_sock, &buffer_size, buffer_size,
				   0, (sockaddr *)&remote_sock_addr, length);
// 161, 97, 48,163,102,99,111,111,114,100,115,162,97,120,24,35,97,121,24,45,98,105,112,105,49,50,55,46,48,46,48,46,49,99,117,105,100
			// send buffer content
			sendto(remote_sock, cbor_map.data(), buffer_size,
				   0, (sockaddr *)&remote_sock_addr, length);
		}

		if (strcmp(command_buffer, "q") == 0 ||
			strcmp(command_buffer, "exit") == 0)
		{
			break;
		}
	} // while

	if (shutdown(listen_sock, SHUT_RDWR) != 0)
	{
		exit(-4);
	}
	close(listen_sock);

	exit(0);
}