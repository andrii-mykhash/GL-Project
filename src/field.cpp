#include "../inc/field.h"

#include <unistd.h>
#include <algorithm>
#include <iostream>

int Field::id_count = 0;

int Field::createUser(std::string ip)
{
    /*
        Create new user with random Dot position, return user_id (uid)
    */
    std::default_random_engine e1(this->rand_device());
    std::uniform_int_distribution<int> x(0, 80);
    std::uniform_int_distribution<int> y(0, 24);
    User temp_user;
    temp_user.uid = Field::id_count;

    bool isnt_done = true;
    while (isnt_done)
    {
        temp_user.coords.x = x(e1);
        temp_user.coords.y = y(e1);
        if (!hasCollision(temp_user))
        {
            isnt_done = false;
        }
    }

    users[temp_user.uid] = temp_user;
    Field::id_count++;
    return temp_user.uid;
}

bool Field::hasCollision(User& to_verify) const
{
    /*
        Check if given position has collision in 'users' collecion
    */

    for (auto it = users.begin(); it != users.end(); it++)
    {
        if (it->second.coords == to_verify.coords && to_verify.uid == it->first)
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
        std::cout << it->second.coords.x << "x" << it->second.coords.y << ":\t" << it->second.ip << "\n";
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
        paint[it->second.coords.x][it->second.coords.y] = '*';
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

void Field::move(User& to_move)
{
    /*
        Change position in current user
    */
    if (!hasCollision(to_move))
    {
        users[to_move.uid] = to_move;
    }
}

Dot Field::getCoords(int& id)
{
    /*
        Return current user coordinates
    */
    return users[id].coords;
}


User Field::getUser(int& id)
{
    /*
        Return user by id
    */
    return users[id];
}

void Field::remove(User u)
{
    /*
        Delete user from collection
    */
    users.erase(u.uid);
}