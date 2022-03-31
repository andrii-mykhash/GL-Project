#include "client/client.h"
#include "json_wrapper.hpp"

#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <map>

int Client::init(std::string ip)
{
    int rec = 0;
    int id = 0;
    if((rec = connectToServer(ip)) < 0)
    {
        return rec;
    }
    rec = recv(server_sock, &id, sizeof(id), 0);
    if (rec != sizeof(id))
    {
        int err = errno;
        std::string str = "recv: id - ";
        str.append(std::to_string(rec))
            .append(" != 4 - sizeof(int)\n strerror= ").append(strerror(err));
        throw std::runtime_error(str);
    }
    f_drawer.init(id);

    setupMulticast();
    createRecvMapThread();   
    return 0;
}

int Client::connectToServer(std::string ip)
{
    sockaddr_in serv_addr;
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        return NetworkError::SOCKET_NOT_CREATED;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(TCP_PORT);

    if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0)
    {
        return NetworkError::INVALID_ADDRESS;
    }

    if (connect(server_sock, (sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        return NetworkError::CANNOT_CONNECT;
    }
    return 0;
}

Client::~Client()
{
    if(map_tread.joinable())
    {
        map_tread.join();
    }
    if (shutdown(server_sock, SHUT_RDWR) != 0)
    {
        int err = errno;
        fprintf(stderr,"server_sock socket shutdown failed: errno=%i, str=\'%s\'\n",err, strerror(err));
    }
    close(server_sock);
    close(multicast_sock);
}

void Client::setupMulticast()
{
    multicast_sock = socket(AF_INET, SOCK_DGRAM,0);
    int one = 1, ret = 0;
    socklen_t sock_len;
    char ttl = 5;
    ip_mreq multicast;

    ret = setsockopt(multicast_sock, SOL_SOCKET, SO_REUSEADDR,
		    (const void *)(&one), sizeof(one));
    if(ret == -1){ printf("error: SOL_SOCKET, SO_REUSEADDR");}

    memset(&multicast_addr, 0, sizeof(multicast_addr));
    multicast_addr.sin_family = AF_INET;
    multicast_addr.sin_port = htons(UDP_PORT);
    multicast_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    sock_len = sizeof(multicast_addr);
    
    ret = bind(multicast_sock, (sockaddr*)&multicast_addr, sock_len);
    if(ret == -1){ printf("set_mult1:  bind(multicast_sock)");}

    multicast.imr_interface.s_addr = inet_addr("0.0.0.0");
    multicast.imr_multiaddr.s_addr = inet_addr(MULTICAST_IP);

    ret = setsockopt(multicast_sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));
    if(ret == -1){ printf("set_mult2:  IPPROTO_IP, IP_MULTICAST_TTL");}
    ret = setsockopt(multicast_sock, IPPROTO_IP,
        IP_ADD_MEMBERSHIP, &multicast, sizeof(multicast));
    if(ret == -1){ printf("set_mult3: IPPROTO_IP, IP_ADD_MEMBERSHIP");}

}

void Client::createRecvMapThread()
{
    map_tread = std::thread([&](){
        while(1)
        {
            usleep(MICROSEC_WAIT);
            f_drawer.setMap(recvMap());
            if(move_char == ComandKeys::EXIT)
            { 
                break;
            }
            f_drawer.draw();
        }
    });
}

std::map<int, User> Client::recvMap()
{
    size_t buff_size = 0;
    int rec = 0;
    socklen_t sock_len = sizeof(multicast_addr);
    cbor_data.clear();
    std::map<int, User> user;
    rec = recvfrom(multicast_sock, &buff_size, sizeof(buff_size), 0,
                (sockaddr *)&multicast_addr, &sock_len);
    if (rec == -1){
		int err = errno;
		fprintf(stderr, "recvMap1. Error: errno=%i, str: %s\n", err, strerror(err));
	}
    cbor_data.resize(buff_size);

    rec = recvfrom(multicast_sock, cbor_data.data(), buff_size, 0,
                (sockaddr *)&multicast_addr, &sock_len);
    if (rec == -1){
		int err = errno;
		fprintf(stderr, "recvMap2. Error: errno=%i, str: %s\n", err, strerror(err));
	}
    if((size_t)rec != buff_size)
    {
        fprintf(stderr,"recvMap3. rec != buff_size: %i != %lu\n", rec, buff_size);
    }
    try{
        user = json_to_map(cbor_data);
    }catch(...)
    {
        user.clear();
    }
    return user;
}

bool Client::isMovableChar(char move_offset) const
{
    return (move_offset == ComandKeys::UP)
               ? false : (move_offset == ComandKeys::LEFT)
               ? false : (move_offset == ComandKeys::DOWN)
               ? false : (move_offset == ComandKeys::RIGHT)
               ? false : (move_offset == ComandKeys::EXIT)
               ? false : true;
}

void Client::sendMoveDirection(char move_offset)
{
    move_char = move_offset;
    send(server_sock, &move_offset, sizeof(move_offset), 0);
}
