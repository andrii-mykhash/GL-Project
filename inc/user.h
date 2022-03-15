#ifndef GL_USER_H
#define GL_USER_H

#include "dot.h"
#include <string>

struct User
{
    int uid;
    std::string ip;
    Dot coords;
};

#endif