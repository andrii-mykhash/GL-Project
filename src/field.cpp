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
    std::uniform_int_distribution<int> x(0, Field::WIDTH);
    std::uniform_int_distribution<int> y(0, Field::HEIGHT);
    User temp_user;
    temp_user.ip = ip;
    bool isnt_done = true;
    std::lock_guard<std::mutex> lock(map_mutex);
    temp_user.uid = Field::id_count;
    Field::id_count++;
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
    return temp_user.uid;
}

bool Field::hasCollision(User& to_verify) const
{
    /*
        Check if given position has collision in 'users' collecion
    */
    std::lock_guard<std::mutex> lock1(collision_mutex);
    for (auto it = users.begin(); it != users.end(); it++)
    {
        if ((it->second.coords <=> to_verify.coords) == 0
             && to_verify.uid != it->first)
        {
            return true;
        }
    }
    return false;
}

int Field::move(User& to_move)
{
    /*
        Change position in current user
    */
    if (!hasCollision(to_move))
    {
        std::lock_guard<std::mutex> lock(map_mutex);
        users[to_move.uid] = to_move;
        return 0;
    }
    return 1;
}

int Field::getUser(int id, User& to_return)
{
    /*
        Return user by id
    */
   try{
        to_return = users.at(id);
    }catch(...){
        return -1;
    }
    return 0;
}

void Field::remove(User u)
{
    /*
        Delete user from collection
    */
    std::lock_guard<std::mutex> lock(map_mutex);
    users.erase(u.uid);
}

std::map<int, User> Field::getMap()
{
    return users;
}