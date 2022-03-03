#include "../inc/field.h"

#include <unistd.h>
#include <algorithm>
#include <iostream>

// #include <fcntl.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <termios.h>
// #include <string.h>

void Field::createUser(std::string ip)
{
    /*
        Create new user with random Dot position
    */
    std::random_device rand_device;

    std::default_random_engine e1(rand_device());
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

bool Field::hasCollision(std::string ip, Dot to_verify) const
{
    /*
        Check if given position has collision in 'users' collecion 
    */
    for (auto it = users.begin(); it != users.end(); it++)
    {
        if (it->second == to_verify && ip.compare(it->first) != 0)
        {
            return true;
        }
    }
    return false;
}

void Field::showAllUsers() const
{
    /*
        Outtut all ouser to stdout
    */
    for (auto it = users.cbegin(); it != users.cend(); ++it)
    {
        std::cout << it->second.x << "x" << it->second.y << ":\t" << it->first << "\n";
    }
}

void Field::draw(int& fd_tty) const
{
    /*
        Output field that have got 80x24 dimention and contain all curent users 
    */
    char paint[80][24] = {'\0'};
    for (auto it = users.cbegin(); it != users.cend(); ++it)
    {
        paint[it->second.x][it->second.y] = '*';  
    }

    // Draw grid
    // +----+
    // |    |
    // |    |
    // +----+
    write(fd_tty,"+", sizeof(char));
    for(int i = 0; i < 80; i++)
    {
        write(fd_tty,"-", sizeof(char));
    }
    write(fd_tty,"+\n", 2*sizeof(char));

    for(int y = 0; y < 24; y++){
        write(fd_tty,"|", sizeof(char));

        for(int x = 0; x < 80; x ++)
        {
            if (paint[x][y] == '\0')
            {
                write(fd_tty," ", sizeof(char));
            }else{
                write(fd_tty,&paint[x][y], sizeof(char));
            }
        }
        write(fd_tty,"|\n", 2*sizeof(char));
    }

    write(fd_tty,"+", sizeof(char));

    for(int i = 0; i < 80; i++)
    {
        write(fd_tty,"-", sizeof(char));
    }
    write(fd_tty,"+\n", 2*sizeof(char));
}

void Field::move(std::string ip_to_move, Dot new_coords)
{
    /*
        Change position in current user
    */
    if (!hasCollision(ip_to_move, new_coords))
    {
        users[ip_to_move] = new_coords;
    }
}

Dot Field::getCoords(std::string ip)
{
    /*
        Return current user coordinates
    */
    return users[ip];
}

void Field::remove(User u)
{
    /*
        Delete user from collection
    */
    users.erase(u.ip);
}