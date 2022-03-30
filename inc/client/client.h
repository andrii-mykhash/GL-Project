#ifndef GL_CLIENT_H
#define GL_CLIENT_H

#include "user.hpp"
#include "both_data.h"

#include <string>
#include <map>
#include <vector>
#include <mutex>
#include <fstream>
#include <memory>
#include <thread>
#include <atomic>
#include <netinet/in.h>

class Client
{
public:
    Client(std::string ip);
    
    ~Client();

    void createRecvMapThread();

    bool isMovableChar(char move_offset) const;

    void sendMoveDirection(char move_offset);

private:
    void draw();

    void initTTY();

    void printAllUsers();

    void drawField();

    void setupMulticast();

    void recvMap();


private:
    int server_sock;
    int multicast_sock;
    sockaddr_in multicast_addr;
	const char *MULTICAST_IP = "229.0.0.80";
    const int TCP_PORT = 8088;
    const int UDP_PORT = 8182;

    int id;
    std::atomic<char> move_char;
    std::thread map_tread;
    std::mutex map_mutex;
    
    User currend_user;
    std::map<int, User> users;
    std::vector<std::uint8_t> cbor_data;
    std::unique_ptr<std::fstream> tty;

    const char *CYAN = "\033[0;36m";
    const char *RESET = "\033[0m";
};

#endif
