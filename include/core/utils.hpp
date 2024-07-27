#pragma once

#include <functional>

namespace heh
{
// Combines multiple hash values into one hash value.
// From: https://stackoverflow.com/a/57595105
template <typename T, typename... Rest>
void hash_combine(std::size_t& seed, const T& value, const Rest&... rest) {
  seed ^= std::hash<T>{}(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  (hash_combine(seed, rest), ...);
}
  
} // namespace heh