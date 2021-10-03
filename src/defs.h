#ifndef DEFS_H
#define DEFS_H

#include <utility>

#define UNUSED(x) (void)(x)
#define VISPLOGID (42)

template <typename T>
auto findMinMax(T* array, std::size_t len) -> std::pair<T, T> {
  T min{array[0]}, max{array[0]};
  for (std::size_t i{0}; i < len; ++i) {
    if (array[i] < min) { min = array[i]; }
    if (array[i] > max) { max = array[i]; }
  }
  return {min, max};
}

#endif
