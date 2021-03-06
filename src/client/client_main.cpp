/// @file client_main.cpp
#include "client/client.h"

#include <iostream>

int main()
{
    Client client;
    int ret_code;
    std::string ip_str;
    bool is_correct_addres = false;
    
    while(!is_correct_addres)
    {
        std::cout << "Write IP to connect (or \'q\'): ";
        std::cin >> ip_str;
        if(ip_str == "q")
        {
            std::cout << "Exiting...\n";
            return 0;
        }
        ret_code = client.init(ip_str);
        switch (ret_code)
        {
        case NetworkCode::CANNOT_CONNECT :
            std::cout << "Cannot connect to the server.\nexiting...\n";
            return ret_code;
        
         case NetworkCode::INVALID_ADDRESS :
            std::cout << "Invalid address\n";
            continue;
        
         case NetworkCode::SOCKET_NOT_CREATED :
            std::cout << "Cannot create socket.\nexiting...\n";
            return ret_code;
        
        case 0:
            is_correct_addres = true;
            break;
        }
    }

    unsigned char move_offset = '\0';
    while (1)
    {
        do
        {
            std::cin >> move_offset;
        } while (!Client::isMoveCorrectChar(move_offset));
        client.sendMoveDirection(move_offset);
        if(move_offset == CommandKeys::EXIT){ break; }
        move_offset = '\0';
    }
    std::cout << std::endl;
    return 0; 
}
