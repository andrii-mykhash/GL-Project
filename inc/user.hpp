#ifndef GL_USER_H
#define GL_USER_H

#include "dot.h"

#include <string>

struct User
{
    int uid;
    std::string ip;
    Dot coords;
    std::strong_ordering operator<=> (const User to_compare) const
    {
        return this->uid <=> to_compare.uid;
    } 
    bool operator== (const User to_compare) const
    {
        return this->uid == to_compare.uid;
    }  
};

#endif