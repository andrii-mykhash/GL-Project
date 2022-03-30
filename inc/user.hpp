#ifndef GL_USER_H
#define GL_USER_H

#include "dot.h"

#include <string>

struct User
{
    int uid;
    std::string ip;
    Dot coords;
    std::strong_ordering operator<=> (const User rhs) const
    {
        return this->uid <=> rhs.uid;
    } 
    bool operator== (const User rhs) const
    {
        return this->uid == rhs.uid;
    }  
};

#endif