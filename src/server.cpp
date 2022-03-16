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
	Field field;
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

	// const int buf_len = 8;
	char command_buffer;
	
	{
		// creating new user
		char str[INET_ADDRSTRLEN] = {0};
		inet_ntop(AF_INET, &(remote_sock_addr.sin_addr), str, INET_ADDRSTRLEN);
		int id = field.createUser(std::string(str));
		send(remote_sock, &id, sizeof(id),0);//, (sockaddr*)&remote_sock_addr,length);//[0], str[INET_ADDRSTRLEN-1]));
	}

	int ret;
	std::map<int, User> to_send = field.getMap();
	// unsigned char coord_buffer[2] = {0};
	size_t map_buffer_size;
	// field.move()
	while (1)
	{
		to_send = field.getMap();
		// send json with map data
		std::vector<std::uint8_t> cbor_map = map_to_json(to_send);
		map_buffer_size = cbor_map.size();

		send(remote_sock, &map_buffer_size, sizeof(map_buffer_size),0);//, (sockaddr *)&remote_sock_addr, length);
		send(remote_sock, &cbor_map.data()[0], map_buffer_size, 0);//, (sockaddr *)&remote_sock_addr, length);
		
		User temp;
		// rec id
		ret = recv(remote_sock, &temp.uid, sizeof(temp.uid), 0);//,(sockaddr *)&remote_sock_addr, &length);
		printf("ret=%d buff_id=%d\n", ret, temp.uid);
		//rec command
		ret = recv(remote_sock, &command_buffer, sizeof(command_buffer), 0);//,(sockaddr *)&remote_sock_addr, &length);
		printf("ret=%d buff_char=%c\n", ret, command_buffer);
		field.getUser(temp.uid, temp);

		switch (command_buffer)
        {
            case 'q':
                goto QUIT_FROM_LOOP;
            break;

            case 's':
                temp.coords.y++;
                temp.coords.y = (temp.coords.y > 23) ?
                         0 : (temp.coords.y < 0) ? 23 : temp.coords.y;
            break;

            case 'a':
                temp.coords.x--;
                temp.coords.x = (temp.coords.x > 79) ? 
                        0 : (temp.coords.x < 0) ? 79 : temp.coords.x;
            break;

            case 'w':
                temp.coords.y--;
                temp.coords.y = (temp.coords.y > 23) ? 
                        0 : (temp.coords.y < 0) ? 23 : temp.coords.y;
            break;

            case 'd':
                temp.coords.x++;
                temp.coords.x = (temp.coords.x > 79) ? 
                        0 : (temp.coords.x < 0) ? 79 : temp.coords.x;

            break;

        }
		field.move(temp);
		command_buffer = '\0';
	} 

QUIT_FROM_LOOP:

	if (shutdown(listen_sock, SHUT_RDWR) != 0)
	{
		exit(-4);
	}
	close(listen_sock);

	exit(0);
}

/*
	  
*/