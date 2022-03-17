#ifndef GL_CLIENT_H
#define GL_CLIENT_H

#include <string>
#include <map>
#include <vector>
#include <mutex>
#include "user.hpp"
#include <fstream>
#include <memory>

const int PORT = 8088;

class UserClient
{
private:
    int sock;
    int ret;
    int id;

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

    // const char *RED = "\033[0;31m";
    // const char *GREEN = "\033[1;32m";
    // const char *YELLOW = "\033[1;33m";
    const char *CYAN = "\033[0;36m";
    // const char *MAGENTA = "\033[0;35m";
    const char *RESET = "\033[0m";

public:
    UserClient(std::string ip, const int port);
    
    ~UserClient();

    void recvMap();

    void draw();

    bool isMovableChar(char move_offset);

    void sendMoveDirection(char move_offset);

private:
    void initTTY();

    void recvID();

    void printAllUsers();

    void drawField();
};

#endif
