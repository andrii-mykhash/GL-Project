#ifndef GL_BOTH_DATA_H
#define GL_BOTH_DATA_H

enum ComandKeys : char
{
    LEFT = 'a',
    RIGHT = 'd',
    EXIT = 'q',
    DOWN = 's',
    UP = 'w'
};

enum NetworkError
{
    CANNOT_CONNECT = -3,
    LISTEN_ERROR = CANNOT_CONNECT,
    INVALID_ADDRESS = -2,
    BIND_ERROR = INVALID_ADDRESS,
    SOCKET_NOT_CREATED = -1
};

constexpr int MICROSEC_WAIT = 100000;

const int FIELD_WIDTH = 78;
const int FIELD_HEIGHT = 22;
#endif