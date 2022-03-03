#ifndef GL_USER
#define GL_USER

#include "dot.h"
#include <string>

struct User
{
    int uid;
    std::string ip;
    Dot coords;
};

#endif