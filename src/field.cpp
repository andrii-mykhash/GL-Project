/// @file field.cpp
#include "field.h"

#include <unistd.h>
#include <algorithm>
#include <iostream>

int Field::id_counter = 0;

/**
 * @brief Create new user with random Dot position.
 * 
 * @param ip_address ip:port that will be attached to the user 
 * @return user id
 */
int Field::createUser(std::string ip_address)
{
    std::default_random_engine e1(this->rand_device());
    std::uniform_int_distribution<int> x(0, FIELD_WIDTH);
    std::uniform_int_distribution<int> y(0, FIELD_HEIGHT);
    User temp_user;
    temp_user.ip = ip_address;
    bool is_done = false;
    std::lock_guard<std::mutex> lock(map_mutex);
    temp_user.uid = Field::id_counter;
    Field::id_counter++;
    while (!is_done)
    {
        temp_user.coords.x = x(e1);
        temp_user.coords.y = y(e1);
        if (!hasCollision(temp_user))
        {
            is_done = true;
        }
    }

    users_info[temp_user.uid] = temp_user;
    return temp_user.uid;
}

/**
 * @brief Check if given position has collision in std::map<int,User> collection.
 * 
 * @param to_verify User struct that will be compared with field data
 * @return true if collision exist
 */
bool Field::hasCollision(User& to_verify) const
{
    std::lock_guard<std::mutex> lock1(collision_mutex);
    for (auto it = users_info.begin(); it != users_info.end(); it++)
    {
        /// If \"to_verify\" Dot and one of std::map \"users_info\" Dot have same coordinates, but different ID -> collision  
        if ((it->second.coords <=> to_verify.coords) == 0
             && to_verify.uid != it->first)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Change user position.
 * 
 * @param to_move User struct with new possition
 * @return 0 in success
 */
int Field::move(User& to_move)
{
    /*
      
    */
    if (!hasCollision(to_move))
    {
        std::lock_guard<std::mutex> lock(map_mutex);
        users_info[to_move.uid] = to_move;
        return 0;
    }
    return -1;
}

/**
 * @brief Return user by id.
 * 
 * @param id User id
 * @param to_return User struct to store return data
 * @return 0 in success
 */
int Field::getUser(int id, User& to_return)
{
   try{
        to_return = users_info.at(id);
    }catch(...){
        return -1;
    }
    return 0;
}

/**
 * @brief Remove user from collection.
 * 
 * @param to_remove User struct
 */
void Field::remove(User to_remove)
{
    std::lock_guard<std::mutex> lock(map_mutex);
    users_info.erase(to_remove.uid);
}
