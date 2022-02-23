#ifndef GL_USER
#define GL_USER
#include "dot.hpp"
#include <string.h>

struct User
{
    char* ip;
    Dot coords;
    
    // bool operator==(const User& rhs)
    // {
    //     return this->coords == rhs.coords && strcmp
    // }
};
#endif