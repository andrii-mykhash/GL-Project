#include <arpa/inet.h>
#include <unistd.h>

#include "../inc/client.h"
#include "../inc/json_wrapper.hpp"
#include <errno.h>

Client::Client(std::string ip)
{
    sockaddr_in serv_addr;
    int rec = 0;
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        int err = errno;
        std::string str = "socket: strerror= ";
        str.append(strerror(err));
        throw std::runtime_error(str);
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(TCP_PORT);

    if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0)
    {
        int err = errno;
        std::string str = "inet_pton: invalid address, strerror= ";
        str.append(strerror(err));
        throw std::runtime_error(str);
    }

    if (connect(server_sock, (sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        int err = errno;
        std::string str = "connect: strerror= ";
        str.append(strerror(err));
        throw std::runtime_error(str);
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
    initTTY();
    setupMulticast();
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
            sleep(1);
            recvMap();
            if(move_char == 'q')
            { 
                break;
            }
            draw();
            *tty << "Move - w/a/s/d; Quit - q: ";
            tty->flush();
        }
        *tty << '\n';
        tty->flush();
    });
}

void Client::recvMap()
{
    size_t buff_size = 0;
    int rec = 0;
    socklen_t sock_len = sizeof(multicast_addr);
    cbor_data.clear();

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
    std::lock_guard<std::mutex> lock(map_mutex);
    try{
        users = json_to_map(cbor_data);
    }catch(...)
    {
        users.clear();
    }
}

bool Client::isMovableChar(char move_offset) const
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
    int i = 1;
    for (auto it = users.cbegin(); it != users.cend(); ++it)
    {
        if (it->first == id)
        {
            *tty << i++ << ")\t" << CYAN << it->second.coords.x << "x"
                 << it->second.coords.y << ":\t" << it->second.ip << "\n"
                 << RESET;
            continue;
        }
        *tty << i++ << ")\t" << it->second.coords.x << "x"
             << it->second.coords.y << ":\t" << it->second.ip << "\n";
    }
}

void Client::drawField()
{
    /*
        Output field that have got 80x24 dimention and contain all curent users
    */
    char paint[FIELD_WIDTH][FIELD_HEIGHT] = {'\0'};
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
    for (int i = 0; i < FIELD_WIDTH; i++)
    {
        *tty << "-";
    }
    *tty << "+\n";

    for (int y = 0; y < FIELD_HEIGHT; y++)
    {
        *tty << "|";
        for (int x = 0; x < FIELD_WIDTH; x++)
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
    for (int i = 0; i < FIELD_WIDTH; i++)
    {
        *tty << "-";
    }
    *tty << "+\n";
}
