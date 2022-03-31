#include "client/field_drawer.h"
#include "both_data.h"

FieldDrawer::~FieldDrawer()
{
    *tty << FieldDrawer::RESET_TERMINAL_VALUE;
}

void FieldDrawer::init(int _id)
{
    id = _id;
    initTTY();
    *tty << FieldDrawer::EXPAND_TERMINAL_VALUE;
}

void FieldDrawer::setMap(std::map<int, User> to_assign)
{
    users.clear();
    users = to_assign;
}

void FieldDrawer::initTTY()
{//                        /dev/stdout    /dev/tty
    tty = std::make_unique<std::fstream>("/dev/stdout", std::ios::binary | std::ios::in | std::ios::out);
            // tty->in | tty->out | tty->trunc);
    if (!tty->is_open())
    {
        throw std::runtime_error("tty open error");
    }
}

void FieldDrawer::draw()
{
    *tty << FieldDrawer::CLEAR_TERMINAL_VALUE;
    printAllUsers();
    drawField();
    *tty << "\r[Move - w|a|s|d] [Quit - q]: ";
    tty->flush();
}

void FieldDrawer::printAllUsers()
{
    int i = 1;
    for (auto it = users.cbegin(); it != users.cend(); ++it)
    {
        if (it->first == id)
        {
            *tty << "\r" << i++ << ")\t" << CYAN << it->second.coords.x << "x"
                 << it->second.coords.y << ":\t" << it->second.ip << "\n"
                 << RESET;
            continue;
        }
        *tty << "\r" << i++ << ")\t" << it->second.coords.x << "x"
             << it->second.coords.y << ":\t" << it->second.ip << "\n";
    }
    *tty << std::endl;
}

void FieldDrawer::drawField()
{
    /*
        Output field that have got 80x24 dimention and contain all curent users
    */
    char paint[FIELD_WIDTH][FIELD_HEIGHT] = {'\0'};
    for (auto it = users.cbegin(); it != users.cend(); ++it)
    {
        if (it->first == id)
        {
            paint[it->second.coords.x][it->second.coords.y] = '@';
            continue;
        }
        paint[it->second.coords.x][it->second.coords.y] = '*';
    }

    *tty << "\r+";
    for (int i = 0; i < FIELD_WIDTH; i++)
    {
        *tty << "-";
    }
    *tty << "+\n";

    for (int y = 0; y < FIELD_HEIGHT; y++)
    {
        *tty << "\r|";
        for (int x = 0; x < FIELD_WIDTH; x++)
        {
            if (paint[x][y] == '\0')
            {
                *tty << " ";
            }
            else
            {
                if (paint[x][y] == '*')
                {
                    *tty << paint[x][y];
                }
                else if (paint[x][y] == '@')
                {
                    *tty << CYAN << paint[x][y] << RESET;
                }
            }
        }
        *tty << "|\n";
    }

    *tty << "\r+";
    for (int i = 0; i < FIELD_WIDTH; i++)
    {
        *tty << "-";
    }
    *tty << "+\n";
}
