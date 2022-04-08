/// @file commondefs.h
#ifndef GL_COMMONDEFS_H
#define GL_COMMONDEFS_H

#include <chrono>

using namespace std::literals::chrono_literals;

/**
 * \enum ::CommandKeys
 * @brief 
 * 
 */
enum CommandKeys : char
{
    LEFT = 'a',
    RIGHT = 'd',
    EXIT = 'q',
    DOWN = 's',
    UP = 'w'
};


/**
 * \enum ::MulticastCode
 * @brief 
 * 
 */
enum NetworkCode
{
    TIME_OUT = -4,
    CANNOT_CONNECT = -3,
    LISTEN_ERROR = CANNOT_CONNECT,
    INVALID_ADDRESS = -2,
    BIND_ERROR = INVALID_ADDRESS,
    SOCKET_NOT_CREATED = -1,
    SUCCESS = 0
};


/**
 * \enum ::MulticastCode
 * @brief 
 * 
 */
enum MulticastCode
{
    LOOP_SETUP_ERROR = -8,
    TTL_SETUP_ERROR = -7,
    MEMBERSHIP_ADD_ERROR = -6,
    REUSE_ADDR_ERROR = -5,
};

constexpr std::chrono::microseconds NOTIFYING_DELAY = 100ms;

const int FIELD_WIDTH = 78;
const int FIELD_HEIGHT = 22;
#endif