#ifndef GL_FIELD
#define GL_FIELD

#include "dot.h"
#include "user.h"

#include <map>
#include <string>
#include <random>
#include <fstream>
// #include <mutex>

class Field
{
private:
    static int id_count;
    std::random_device rand_device;
    std::map<int, User> users;
public:
    bool hasCollision(User& to_verify) const;
    int createUser(std::string ip);
    void move(User& to_move);
    void remove(User to_remove);
    Dot getCoords(int& id);
    User getUser(int& id);

    // to remove
    void draw(std::fstream* tty);
    void showAllUsers() const;
};


#endif