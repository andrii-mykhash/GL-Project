#ifndef GL_FIELD
#define GL_FIELD

#include "dot.hpp"
#include "user.hpp"
#include <map>
#include <vector>
// #include <string.h>
#include <mutex>


class Field
{
private:   
    // std::mutex mut;
    std::map<char*, Dot> users;
public:
    bool hasCollision(char* ip, Dot to_verify);
    void createUser(char* ip);
    std::map<Dot, char*> get();
    void move(char*, Dot new_coords);
    void remove(User);
    // to remove
    void add();
    Dot gets(char*);
};

#endif