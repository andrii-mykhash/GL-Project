#include "headers/field.hpp"


// #include <fcntl.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <termios.h>
// #include <string.h>


void Field::createUser(std::string ip)
{
    std::random_device r;

    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> x(0, 80);
    std::uniform_int_distribution<int> y(0, 24);
    Dot coords;

    bool isnt_done = true;
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

bool Field::hasCollision(std::string ip, Dot to_verify)
{
    for (auto it = users.begin(); it != users.end(); it++)
    {
        if (it->second == to_verify && ip.compare(it->first) != 0)
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

void Field::draw(int& tty)
{
    char paint[80][24] = {'\0'};
    for (auto it = users.cbegin(); it != users.cend(); ++it)
    {
        paint[it->second.x][it->second.y] = '*';  
    }

    write(tty,"+", sizeof(char));
    for(int i = 0; i < 80; i++)
    {
        write(tty,"-", sizeof(char));
    }
    write(tty,"+\n", 2*sizeof(char));

    for(int y = 0; y < 24; y++){
        write(tty,"|", sizeof(char));

        for(int x = 0; x < 80; x ++)
        {
            if (paint[x][y] == '\0')
            {
                write(tty," ", sizeof(char));
            }else{
                write(tty,&paint[x][y], sizeof(char));
            }
        }
        write(tty,"|\n", 2*sizeof(char));
    }

    write(tty,"+", sizeof(char));

    for(int i = 0; i < 80; i++)
    {
        write(tty,"-", sizeof(char));
    }
    write(tty,"+\n", 2*sizeof(char));
}

void Field::move(std::string ip_to_move, Dot new_coords)
{
    if (!hasCollision(ip_to_move, new_coords))
    {
        users[ip_to_move] = new_coords;
    }
}

Dot Field::gets(std::string ip)
{
    return users[ip];
}

void Field::remove(User u)
{
    users.erase(u.ip);
}