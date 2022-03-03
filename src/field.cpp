#include "../inc/field.h"

#include <unistd.h>
#include <algorithm>
#include <iostream>


void Field::createUser(std::string ip)
{
    /*
        Create new user with random Dot position
    */
    std::default_random_engine e1(this->rand_device());
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

void Field::draw(std::fstream *tty)
{
    /*
        Output field that have got 80x24 dimention and contain all curent users
    */

    tty->flush();
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

    *tty << "+";
    for (int i = 0; i < 80; i++)
    {
        *tty << "-";
    }
    *tty << "+\n";

    for (int y = 0; y < 24; y++)
    {
        *tty << "|";
        for (int x = 0; x < 80; x++)
        {
            if (paint[x][y] == '\0')
            {
                *tty << " ";
            }
            else
            {
                *tty << paint[x][y];
            }
        }
        *tty << "|\n";
    }

    *tty << "+";
    for (int i = 0; i < 80; i++)
    {
        *tty << "-";
    }
    *tty << "+\n";
    tty->flush();
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