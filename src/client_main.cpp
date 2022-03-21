#include <iostream>
#include "../inc/client.h"

int main()
{
    std::string ip_str;
    std::cout << "Write IP to connect: ";
    std::cin >> ip_str;

    Client us(ip_str, PORT);

    unsigned char move_offset = '\0';
    while (1)
    {
        us.recvMap();
        us.draw();
        std::cout << "Move - w/a/s/d; Quit - q: ";
        do
        {
            std::cin >> move_offset;
        } while (us.isMovableChar(move_offset));

        us.sendMoveDirection(move_offset);
        if (move_offset == 'q')
        {
            break;
        }
        move_offset = '\0';
    }

    return 0;
}
