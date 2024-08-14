#pragma once

namespace heh {
  constexpr int SqrtInt(int num, int lo = 0, int hi = 0) {
    if (hi == 0) hi = num;
    int mid = (lo + hi) / 2;
    if (mid * mid == num) return mid;
    else if (mid * mid > num) return SqrtInt(num, lo, mid - 1);
    else return SqrtInt(num, mid + 1, hi);
  }

  constexpr bool IsPerfectSquare(int num) {
    if (num < 0) return false;
    int root = SqrtInt(num);
    return root * root == num;
  }
} // namespace heh