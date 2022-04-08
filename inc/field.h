#ifndef GL_FIELD_H
#define GL_FIELD_H

#include "dot.h"
#include "user.hpp"
#include "commondefs.h"

#include <map>
#include <string>
#include <random>
#include <fstream>
#include <mutex>

class Field
{
public:
    bool hasCollision(User& to_verify) const;
    
    int createUser(std::string ip);
    
    int move(User& to_move);
    
    void remove(User to_remove);
    
    int getUser(int id, User& to_return);
    
        
    /// @brief Get map with users data.
    inline std::map<int, User> getMap()
    {
        return users_info;
    }

private:
    static int id_counter;
    std::random_device rand_device;
    std::map<int, User> users_info;
    std::mutex map_mutex;
    mutable std::mutex collision_mutex;
};


#endif