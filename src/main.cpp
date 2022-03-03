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
    f.createUser("10.10.11.12");
    f.createUser("1.16.3.11");
    f.showAllUsers();

    while (true)
    {
        f.draw(tty);
        Dot ddd = f.getCoords("10.10.11.12");
        format_out = ddd.x + " " + ddd.y;
        *tty << "\n" << ddd.x << "x" << ddd.y << std::endl;
        int x, y;
        
        std::cin >> x >> y;
        f.move("10.10.11.12",{x,y});

        *tty << "\033c";
    }

    delete tty;
    return 0;
}
