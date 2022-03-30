#ifndef DEFS_H
#define DEFS_H

#include <math.h>
#include <utility>

#define UNUSED(x)    (void)(x)
#define VISPLOGID    (42)
#define SIGN(x)      (((x) < 0.0) ? -1.0 : 1.0)
#define ABS(x)       (((x) < 0.0) ? -(x) : (x))
#define QLOGSCALE(x) ((SIGN(x) * powf(ABS(x), 0.25f)))

#define STATE_FILENAME ("nttiny.toml")

template <typename T>
auto findMinMax(T* array, std::size_t len, bool use_log) -> std::pair<T, T> {
  T min, max;
  if (use_log) {
    min = QLOGSCALE(array[0]);
    max = QLOGSCALE(array[0]);
    for (std::size_t i{0}; i < len; ++i) {
      if (QLOGSCALE(array[i]) < min) { min = QLOGSCALE(array[i]); }
      if (QLOGSCALE(array[i]) > max) { max = QLOGSCALE(array[i]); }
    }
  } else {
    min = array[0];
    max = array[0];
    for (std::size_t i{0}; i < len; ++i) {
      if (array[i] < min) { min = array[i]; }
      if (array[i] > max) { max = array[i]; }
    }
  }
  return {min, max};
}

#endif
