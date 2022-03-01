#ifndef GL_DOT
#define GL_DOT

struct Dot
{
    int x;
    int y;

    bool operator< (const Dot& to_verify) const;
    bool operator== (const Dot& to_verify) const;
};

#endif