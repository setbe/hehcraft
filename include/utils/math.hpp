#pragma once

// std
#include <type_traits>

namespace heh::math {

  template <typename T_int>
  consteval int SqrtInt(T_int num, T_int lo = 0, T_int hi = 0) {
    if (hi == 0) hi = num;
    T_int mid = (lo + hi) / 2;
    if (mid * mid == num) return mid;
    else if (mid * mid > num) return SqrtInt<T_int>(num, lo, mid - 1);
    else return SqrtInt<T_int>(num, mid + 1, hi);
  }

  template <typename T_int>
  consteval bool IsPerfectSquare(T_int num) {
    if (num < 0) return false;
    T_int root = SqrtInt<T_int>(num);
    return root * root == num;
  }

} // namespace heh