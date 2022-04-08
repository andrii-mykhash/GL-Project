///@file dot.cpp
#include "dot.h"

/**
 * @brief Dots comparison.
 * 
 * E.g.: [Dot(32,5) <=> Dot(8, 76)] --> [int(3205) <=> int(876)].
 */
std::weak_ordering Dot::operator<=>(const Dot &to_verify) const
{
  int lhs_temp, rhs_temp;
  const int shift_to_left = 100;
  lhs_temp = this->x * shift_to_left + this->y;
  rhs_temp = to_verify.x * shift_to_left + to_verify.y;

  return lhs_temp <=> rhs_temp;
}
