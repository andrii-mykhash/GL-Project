#include <iostream>
#include "../inc/field.h"

#include <string.h>
#include <fstream>
#include <iostream>

#include <unistd.h>


int main()
{
    std::fstream* tty = new std::fstream("/dev/tty", tty->in | tty->out | tty->trunc);
    if(!tty->is_open())
    {
        std::cerr << "tty open error" << std::endl;
        exit(-1);
    }

    std::string format_out = "";

    Field f;
    
    f.createUser("192.168.0.11");
    f.createUser("8.172.5.15");
    User temp_user;
    temp_user.uid = f.createUser("10.10.11.12");
    f.createUser("1.16.3.11");
    f.showAllUsers();
    
    while (true)
    {

        f.draw(tty);
        Dot ddd = f.getCoords(temp_user.uid);
        format_out = ddd.x + " " + ddd.y;
        *tty << "\n" << ddd.x << "x" << ddd.y << std::endl;
        // int x, y;
        if(!std::cin.good())
        {
            break;
        }
        std::cin >> temp_user.coords.x >> temp_user.coords.y;
        f.move(temp_user);

        *tty << "\033c";
    }

    delete tty;
    return 0;
}
