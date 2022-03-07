#include "../inc/dot.h"

std::weak_ordering Dot::operator<=>(const Dot &to_verify) const
{
  int lhs_temp, rhs_temp;
  lhs_temp = this->x * 100 + this->y;
  rhs_temp = to_verify.x * 100 + to_verify.y;

  return lhs_temp <=> rhs_temp;
}
