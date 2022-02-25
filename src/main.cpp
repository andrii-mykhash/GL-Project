#include <iostream>
#include "headers/field.hpp"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>
#include <unistd.h>

int setup_tty_terminal()
{
    int terminal = open("/dev/tty", O_RDWR | O_NOCTTY);

    struct termios tty;
    memset(&tty, 0, sizeof(tty));

    if (tcgetattr(terminal, &tty) != 0)
    {
        std::cout << "Error " << errno << " from tcgetattr: " << strerror(errno) << std::endl;
    }

    cfsetospeed(&tty, (speed_t)B9600);
    cfsetispeed(&tty, (speed_t)B9600);

    tty.c_cc[VMIN] = 1; 

    tcflush(terminal, TCIFLUSH);
    if (tcsetattr(terminal, TCSANOW, &tty) != 0)
    {
        std::cout << "Error " << errno << " from tcsetattr" << std::endl;
    }
    return terminal;
}

int main()
{
    int tty = setup_tty_terminal();
    std::string format_out = "";

    Field f;
    
    f.createUser("192.168.0.11");
    f.createUser("8.172.5.15");
    f.createUser("10.10.11.12");
    f.createUser("1.16.3.11");
    f.add();

    while (true)
    {
        f.draw(tty);
        Dot ddd = f.gets("10.10.11.12");
        format_out = ddd.x + " " + ddd.y;
        write(tty,format_out.c_str(),(sizeof(char)*format_out.size()));
        // std::cout << "\n" << ddd.x << "x" << ddd.y << std::endl;
        format_out.erase();
        int x, y;
        std::cin >> x >> y;
        // read(tty,&x,sizeof(int));
        // read(tty,&y,sizeof(int));
        f.move("10.10.11.12",{x,y});
        system("clear");
    }
    return 0;
}
