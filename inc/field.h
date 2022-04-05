#ifndef GL_FIELD_H
#define GL_FIELD_H

#include "dot.h"
#include "user.hpp"
#include "both_data.h"

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
    
    std::map<int, User> getMap();

private:
    static int id_counter;
    std::random_device rand_device;
    std::map<int, User> users_info;
    std::mutex map_mutex;
    mutable std::mutex collision_mutex;
};


#endif