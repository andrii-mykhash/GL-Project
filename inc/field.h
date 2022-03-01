#ifndef GL_FIELD
#define GL_FIELD

#include "../inc/dot.h"
#include "../inc/user.h"

#include <map>
#include <string>
#include <random>
#include <mutex>


class Field
{
private:   
    std::map<std::string, Dot> users;
public:
    bool hasCollision(std::string ip, Dot to_verify);
    void createUser(std::string ip);
    void move(std::string, Dot new_coords);
    void remove(User);
    // to remove
    void draw(int&);
    void add();
    Dot gets(std::string);
};

#endif