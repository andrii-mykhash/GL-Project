#ifndef GL_FIELD_DRAWER_H
#define GL_FIELD_DRAWER_H

#include "user.hpp"

#include <memory>
#include <map>
#include <fstream>

class FieldDrawer
{
public:
    void init(int _id);

    void draw();

    void setMap(std::map<int, User> to_assign);

    ~FieldDrawer();

private:
    void initTTY();

    void drawField();

    void printAllUsers();

private:
    User currend_user;
    std::map<int, User> users;
    std::unique_ptr<std::fstream> tty;
    int id; 

    inline static const char *CLEAR_TERMINAL_VALUE = "\033c";
    inline static const char *EXPAND_TERMINAL_VALUE = "\e[8;35;80t";    
    inline static const char *RESET_TERMINAL_VALUE = "\e[8;24;80t";
    inline static const char *CYAN = "\033[0;36m";
    inline static const char *RESET = "\033[0m";
};

#endif