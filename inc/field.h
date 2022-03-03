#ifndef GL_FIELD
#define GL_FIELD

#include "dot.h"
#include "user.h"

#include <map>
#include <string>
#include <random>
// #include <mutex>

//TODO: "ip" change to "id"
class Field
{
private:   
    static std::random_device rand_device;
    std::map<std::string, Dot> users;
public:
    bool hasCollision(std::string ip, Dot to_verify) const;
    void createUser(std::string ip);
    void move(std::string ip, Dot new_coords);
    void remove(User to_remove);
    Dot getCoords(std::string ip);

    // to remove
    void draw(int& fd_tty) const;
    void showAllUsers() const;
};

#endif