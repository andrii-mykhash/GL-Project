#ifndef GL_DOT
#define GL_DOT

#include <compare>

struct Dot
{
    int x;
    int y;
    std::weak_ordering operator<=>(const Dot &to_verify) const;
};

#endif