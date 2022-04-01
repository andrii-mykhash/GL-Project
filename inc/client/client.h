#ifndef GL_CLIENT_H
#define GL_CLIENT_H

#include "user.hpp"
#include "both_data.h"
#include "field_drawer.h"

#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <netinet/in.h>

class Client
{
public:    
    ~Client();

    int init(std::string ip);

    bool isMovableChar(char move_char) const;

    void sendMoveDirection(char move_direction);

private:
    void setupMulticast();

    int connectToServer(std::string ip);

    void createRecvMapThread();

    std::map<int, User> recvMap();

private:
    FieldDrawer f_drawer;
    int server_sock;
    int multicast_sock;
    sockaddr_in multicast_addr;

    std::atomic<char> move_char;
    std::thread map_tread;
    
    std::vector<std::uint8_t> cbor_data;
    
	inline static const char *MULTICAST_IP = "229.0.0.80";
    static constexpr int TCP_PORT = 8088;
    static constexpr int UDP_PORT = 8182;
};

#endif
