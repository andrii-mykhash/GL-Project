#include <iostream>
#include "headers/field.hpp"

int main(){
    Dot a;
    a.x = 10;
    a.y = 10;
    Field f;
    f.createUser("192.168.0.11");
    f.createUser("8.172.5.15");
    f.add();
    std::cout << "\n";
    Dot ff = f.gets("192.168.0.11");

    f.move("8.172.5.15", ff);
    f.add();
    std::cout << "\n";

    f.move("8.172.5.15", a);


    f.add();
    std::cout << "\n";

    f.createUser("10.10.11.12");
    f.createUser("1.16.3.11");
    f.add();
    std::cout << "\n";

    return 0;
}