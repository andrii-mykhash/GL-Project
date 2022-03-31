#include "client/client.h"

#include <iostream>

int main()
{
    Client client;
    int ret_code;
    std::string ip_str;

    do{
        std::cout << "Write IP to connect (or \'q\'): ";
        std::cin >> ip_str;

        ret_code = client.init(ip_str);
    }while(ret_code == NetworkError::INVALID_ADDRESS || ip_str == "q");
    if(ip_str == "q")
    { return 0; }
    
    unsigned char move_offset = '\0';
    while (1)
    {
        do
        {
            std::cin >> move_offset;
        } while (client.isMovableChar(move_offset));
        client.sendMoveDirection(move_offset);
        if(move_offset == ComandKeys::EXIT){ break; }
        move_offset = '\0';
    }
    return 0; 
}
