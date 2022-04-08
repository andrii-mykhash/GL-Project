#ifndef GL_DOT_H
#define GL_DOT_H

#include <compare>

struct Dot
{
    int x;
    int y;
    std::weak_ordering operator<=> (const Dot &to_compare) const;
    bool operator== (const Dot &to_compare) const;
};

#endif