#ifndef DEFS_H
#define DEFS_H

#include <math.h>
#include <utility>

#define UNUSED(x)      (void)(x)
#define VISPLOGID      (42)

#ifndef SIGN
  #define SIGN(x)        (((x) < 0.0) ? -1.0 : 1.0)
#endif

#ifndef ABS
  #define ABS(x)         (((x) < 0.0) ? -(x) : (x))
#endif

#ifndef QLOGSCALE
  #define QLOGSCALE(x)   ((SIGN(x) * powf(ABS(x), 0.25f)))
#endif

#define STATE_FILENAME ("nttiny.toml")

#define BELYASH_PINK   (ImVec4(1.0f, 0.745f, 0.745f, 1))

using point_t = float[2];

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

#undef SIGN
#undef ABS
#undef QLOGSCALE

#endif
