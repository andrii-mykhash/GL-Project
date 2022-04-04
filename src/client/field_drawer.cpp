#include "client/field_drawer.h"
#include "both_data.h"

/**
 *  @brief Destroy the FieldDrawer::FieldDrawer object.
 * 
 * 
 *  Destroy object, reset terminal size and set TTY to cooked mode.
 */
FieldDrawer::~FieldDrawer()
{
    *tty << FieldDrawer::RESET_TERMINAL_VALUE;
    system("/bin/stty cooked");
}

/**
 * @brief Initialize main components.
 * 
 * Open and set tty to raw mode, expand terminal size and set user id.
 * 
 * @param[in] _id Current user id
 */
void FieldDrawer::init(int _id)
{
    system("/bin/stty raw");
    openTTYFile();
    id = _id;
    *tty << FieldDrawer::EXPAND_TERMINAL_VALUE;
}

/**
 * @brief Initialize TTY i/o.
 * 
 * Open \"/dev/tty\" file in input/output mode.
 */
void FieldDrawer::openTTYFile()
{//                        /dev/stdout    /dev/tty
    tty = std::make_unique<std::fstream>("/dev/tty", std::ios::binary | std::ios::in | std::ios::out);
    if (!tty->is_open())
    {
        throw std::runtime_error("tty open error");
    }
}

/**
 * @brief Draw UI - users, field and controlls button.
 */
void FieldDrawer::draw()
{
    *tty << FieldDrawer::CLEAR_TERMINAL_VALUE;
    printAllUsers();
    drawField();
    *tty << "\r[Move - w|a|s|d] [Quit - q]: ";
    tty->flush();
}

/**
 * @brief Print info about all users.
 */
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

/**
 * @brief Draw field grid [FIELD_WIDTH][FIELD_HEIGHT] dimmention.
 */
void FieldDrawer::drawField()
{
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
