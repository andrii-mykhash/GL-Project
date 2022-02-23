#include "headers/field.hpp"
#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <random>

void Field::createUser(char *ip)
{
    std::random_device r;

    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> x(0, 80);
    std::uniform_int_distribution<int> y(0, 24);
    // srand(time(NULL));
    Dot coords;
    bool isnt_done = true;
    // strcpy(to_return.ip, ip);
    while (isnt_done)
    {
        coords.x = x(e1);
        coords.y = y(e1);
        if (!hasCollision(ip, coords))
        {
            isnt_done = false;
        }
    }
    users[ip] = coords;
}


bool Field::hasCollision(char* ip, Dot to_verify)
{
    for(auto it = users.begin(); it != users.end(); it++)
    {
        if(it->second == to_verify && strcmp(it->first, ip) != 0)
        {
            return true;
        }
    }
    return false;
}


void Field::add()
{
    for (auto it = users.cbegin(); it != users.cend(); ++it)
    {
        std::cout << it->second.x << "x" << it->second.y << ":\t" << it->first << "\n";
    }
}

void Field::move(char* ip_to_move, Dot new_coords)
{
    if(!hasCollision(ip_to_move,new_coords))
    {
        users[ip_to_move] = new_coords;
    }
}
Dot Field::gets(char* ip)
{
    return users[ip];
}