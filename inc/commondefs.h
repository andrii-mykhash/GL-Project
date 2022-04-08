/// @file commondefs.h
#ifndef GL_COMMONDEFS_H
#define GL_COMMONDEFS_H

#include <chrono>

using namespace std::literals::chrono_literals;

/// \enum ::CommandKeys
enum CommandKeys : char
{
    /// a
    LEFT = 'a',
    /// d
    RIGHT = 'd',
    /// q
    EXIT = 'q',
    /// s
    DOWN = 's',
    /// w
    UP = 'w'
};


/// \enum ::NetworkCode
enum NetworkCode
{
    /// -4
    TIME_OUT = -4,
    /// -3 
    CANNOT_CONNECT = -3,
    /// -3
    LISTEN_ERROR = CANNOT_CONNECT, 
    /// -2
    INVALID_ADDRESS = -2,
    /// -2
    BIND_ERROR = INVALID_ADDRESS,
    /// -1
    SOCKET_NOT_CREATED = -1,
    /// 0
    SUCCESS = 0
};


/// \enum ::MulticastCode
enum MulticastCode
{
    /// -8
    LOOP_SETUP_ERROR = -8, 
    /// -7
    TTL_SETUP_ERROR = -7, 
    /// -6
    MEMBERSHIP_ADD_ERROR = -6, 
    /// -5
    REUSE_ADDR_ERROR = -5, 
};

/** @brief Delay multicast for notifying map to clients
*
*   Not recommended set value < 60ms
*/
constexpr std::chrono::microseconds NOTIFYING_DELAY = 80ms; 

/// @brief Terminal grid width size. Max recommended value 202 (1080p) 
const int FIELD_WIDTH = 78;

/// @brief Terminal grid height size. Max recommended value 40 (1080p) 
const int FIELD_HEIGHT = 22;
#endif