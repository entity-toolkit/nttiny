#ifndef DEFS_H
#define DEFS_H

#include <utility>

#define UNUSED(x) (void)(x)

#define HARD_LIMIT_FPS 120

template<typename T>
std::pair<T, T> findMinMax(T* array, std::size_t len) {
  T min{array[0]}, max{array[0]};
  for (std::size_t i{0}; i < len; ++i) {
    if (array[i] < min) {
      min = array[i];
    }
    if (array[i] > max) {
      max = array[i];
    }
  }
  return {min, max};
}

#endif
