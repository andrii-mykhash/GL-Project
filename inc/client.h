#ifndef GL_CLIENT_H
#define GL_CLIENT_H

#include <string>
#include <map>
#include <vector>
#include <mutex>
#include "user.hpp"
#include <fstream>
#include <memory>
#include <thread>
#include <atomic>

const int PORT = 8088;

class Client
{
public:
    Client(std::string ip, const int port);
    
    ~Client();

    int recvMap();

    void draw();

    bool isMovableChar(char move_offset);

    void sendMoveDirection(char move_offset);

private:
    int connectToServer(std::string ip, const int port);

    void initTTY();

    void printAllUsers();

    void drawField();

private:
    int server_sock;
    int multicast_sock;
    int ret;
    int id;
    std::atomic<char> move_char;
    std::thread map_tread;
    std::mutex map_writer_mutex;
    User currend_user;
    std::map<int, User> users;
    std::vector<std::uint8_t> cbor_data;
    std::unique_ptr<std::fstream> tty;
    std::mutex map_mutex;

    enum ComandKeys
    {
        A = 'a',
        D = 'd',
        Q = 'q',
        S = 's',
        W = 'w'
    };

    const char *CYAN = "\033[0;36m";
    const char *RESET = "\033[0m";
};

#endif
