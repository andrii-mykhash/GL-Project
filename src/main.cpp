#include <iostream>
#include "../inc/field.h"

#include <string.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <unistd.h>


int main()
{
    std::unique_ptr<std::fstream> tty = std::make_unique<std::fstream>("/dev/tty", tty->in | tty->out | tty->trunc);
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
        
        f.draw(tty.get());
        if(f.getUser(temp_user.uid, temp_user) < 0)
        {
            continue;
        }
        format_out = temp_user.coords.x + " " + temp_user.coords.y;
        *tty << "\n - Current pos: " << temp_user.coords.x << "x" << temp_user.coords.y << "\nx/y: ";
        tty->flush();
        if(!std::cin.good())
        {
            break;
        }
        std::cin >> temp_user.coords.x >> temp_user.coords.y;
        f.move(temp_user);

        *tty << "\033c";
    }

    return 0;
}
