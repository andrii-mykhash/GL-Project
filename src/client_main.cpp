#include <iostream>
#include "../inc/client.h"

int main()
{
    std::string ip_str;
    std::cout << "Write IP to connect: ";
    std::cin >> ip_str;

    Client us(ip_str);
    us.createRecvMapThread();
    unsigned char move_offset = '\0';
    std::cout << "\e[8;35;82t";
    while (1)
    {
        do
        {
            std::cin >> move_offset;
        } while (us.isMovableChar(move_offset));
        us.sendMoveDirection(move_offset);
        if(move_offset == 'q'){ break; }
        move_offset = '\0';
    }
    std::cout << "\e[8;24;80t";
    return 0; 
}
