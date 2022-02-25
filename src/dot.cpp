#include "headers/dot.hpp"

bool operator==(const Dot& lhs, const Dot& rhs)
{
    int lhs_temp, rhs_temp;
    lhs_temp = lhs.x*100 + lhs.y;
    rhs_temp = rhs.x*100 + rhs.y;
    return lhs_temp == rhs_temp;
}

bool Dot::operator<(const Dot& rhs) const
{
    int lhs_temp, rhs_temp;
    lhs_temp = this->x*100 + rhs.y;
    rhs_temp = this->x*100 + rhs.y;
    return lhs_temp < rhs_temp;
}
bool Dot::operator==(const Dot& rhs) const
{
    int lhs_temp, rhs_temp;
    lhs_temp = this->x*100 + this->y;
    rhs_temp = rhs.x*100 + rhs.y;
    return lhs_temp == rhs_temp;
}