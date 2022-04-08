/// @file client.cpp
#include "client/client.h"
#include "json_wrapper.hpp"

#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <map>

/**
 * @brief Initialize main components.
 * 
 * Connect to remote server, setup multicast, init field drawer, 
 * get id and create map grabber thread. 
 * 
 * @param[in] ip remote address of server
 * @return 0 in success, one of ::NetworkCode or ::MulticastCode enum code in error
 */
int Client::init(std::string ip, bool test_flag)
{
    int rec = 0;
    int id = 0;
    if((rec = connectToServer(ip)) < 0)
    {
        return -1;
    }
    if(test_flag)
    {
        return 0;
    }
    rec = recv(server_sock, &id, sizeof(id), 0);
    if (rec != sizeof(id))
    {
        return -2;
    }
    field_drawer.init(id);

    setupMulticast();
    grabMapFromMulticastThread();   
    return 0;
}

/**
 * @brief Connect to remote server.
 * 
 * @param[in] ip remote address
 * @return 0 in success, one of ::NetworkCode or ::MulticastCode enum code in error
 */
int Client::connectToServer(std::string ip)
{
    sockaddr_in serv_addr;
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        return NetworkCode::SOCKET_NOT_CREATED;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(TCP_PORT);

    if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0)
    {
        return NetworkCode::INVALID_ADDRESS;
    }

    if (connect(server_sock, (sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        return NetworkCode::CANNOT_CONNECT;
    }
    return NetworkCode::SUCCESS;
}

/**
 * @brief Destroy the Client object.
 * 
 * Join multicast thread, close socket file descriptors. 
 */
Client::~Client()
{
    const int NON_SOCK_OPERATION = 88;
    int err;
    if(map_receiver_tread.joinable())
    {
        map_receiver_tread.join();
        close(multicast_sock);
    }
    if (shutdown(server_sock, SHUT_RDWR) != 0)
    {
        err = errno;
        if(err != NON_SOCK_OPERATION)
        {
            fprintf(stderr,"\nserver_sock: socket shutdown failed: errno=%i, str=\'%s\'\n",err, strerror(err));
        }
    }
    if(err != NON_SOCK_OPERATION)
    {
        close(server_sock);
    }
}

/**
 * @brief Create UDP multicast socket.
 * @return 0 in success, ::NetworkCode::BIND_ERROR or one of ::MulticastCode enum code in error
 */
int Client::setupMulticast()
{
    multicast_sock = socket(AF_INET, SOCK_DGRAM,0);
    int one = 1, ret = 0;
    socklen_t sock_len;
    char ttl = 5;
    ip_mreq multicast;

    ret = setsockopt(multicast_sock, SOL_SOCKET, SO_REUSEADDR,
		    (const void *)(&one), sizeof(one));
    if(ret == -1)
    { 
        int err = errno;
		fprintf(stderr,"[CLIENT] set_mult: SOL_SOCKET, SO_REUSEADDR, errno=%i, str - %s\n", err, strerror(err));
		return MulticastCode::REUSE_ADDR_ERROR;
    }

    memset(&multicast_addr, 0, sizeof(multicast_addr));
    multicast_addr.sin_family = AF_INET;
    multicast_addr.sin_port = htons(UDP_PORT);
    multicast_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    sock_len = sizeof(multicast_addr);
    
    ret = bind(multicast_sock, (sockaddr*)&multicast_addr, sock_len);
    if(ret == -1)
    { 
        int err = errno;
		fprintf(stderr,"[CLIENT] set_mult:  bind(multicast_sock), errno=%i, str - %s\n", err, strerror(err));
		return NetworkCode::BIND_ERROR;
    }

    multicast.imr_interface.s_addr = inet_addr("0.0.0.0");
    multicast.imr_multiaddr.s_addr = inet_addr(MULTICAST_IP);

    ret = setsockopt(multicast_sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));
    if(ret == -1)
    { 
        int err = errno;
		fprintf(stderr,"[CLIENT] set_mult:  IPPROTO_IP, IP_MULTICAST_TTL, errno=%i, str - %s\n", err, strerror(err));
		return MulticastCode::TTL_SETUP_ERROR;
    }

    ret = setsockopt(multicast_sock, IPPROTO_IP,
        IP_ADD_MEMBERSHIP, &multicast, sizeof(multicast));
    if(ret == -1)
    { 
        int err = errno;
		fprintf(stderr,"[CLIENT] set_mult: IPPROTO_IP, IP_ADD_MEMBERSHIP, errno=%i, str - %s\n", err, strerror(err));
		return MulticastCode::MEMBERSHIP_ADD_ERROR;
    }
    return NetworkCode::SUCCESS;
}

/// @brief Create thread that receives std::map<int, User> from multicast
void Client::grabMapFromMulticastThread()
{
    map_receiver_tread = std::thread([&](){
        while(1)
        {
            std::this_thread::sleep_for(NOTIFYING_DELAY);
            field_drawer.setMap(recvMap());
            if(move_char == CommandKeys::EXIT)
            { 
                break;
            }
            field_drawer.draw();
        }
    });
}

/**
 * @brief Receive map with users info from remove server. 
 * 
 * @return non-empty map in success 
 */
std::map<int, User> Client::recvMap()
{
    size_t buff_size = 0;
    int rec = 0;
    socklen_t sock_len = sizeof(multicast_addr);
    cbor_json.clear();
    std::map<int, User> user;
    rec = recvfrom(multicast_sock, &buff_size, sizeof(buff_size), 0,
                (sockaddr *)&multicast_addr, &sock_len);
    if (rec == -1){
		int err = errno;
		fprintf(stderr, "recvMap1. Error: errno=%i, str: %s\n", err, strerror(err));
	}
    cbor_json.resize(buff_size);

    rec = recvfrom(multicast_sock, cbor_json.data(), buff_size, 0,
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
        user = json_to_map(cbor_json);
    }catch(...)
    {
        user.clear();
    }
    return user;
}

/**
 * @brief Check if char belong to ::CommandKeys enum.
 * 
 * Converting a single char to lowercase and comparing.
 * @param[in] to_verify char that need test 
 * @return true if belong to ::CommandKeys enum.
 */
bool Client::isMoveCorrectChar(char to_verify)
{
    to_verify = std::tolower(to_verify);
    return (to_verify == CommandKeys::UP)
               ? true : (to_verify == CommandKeys::LEFT)
               ? true : (to_verify == CommandKeys::DOWN)
               ? true : (to_verify == CommandKeys::RIGHT)
               ? true : (to_verify == CommandKeys::EXIT)
               ? true : false;
}

/**
 * @brief Send char to remote server 
 * 
 * @param[in] move_direction one of ::CommandKeys enum char
 */
void Client::sendMoveDirection(char move_direction)
{
    move_char = move_direction;
    send(server_sock, &move_direction, sizeof(move_direction), 0);
}
