#ifndef DEFS_H
#define DEFS_H

#define UNUSED(x)      (void)(x)
#define VISPLOGID      (42)

#define STATE_FILENAME ("nttiny.toml")

#define BELYASH_PINK   (ImVec4(1.0f, 0.745f, 0.745f, 1))

using point_t = float[2];
using ushort  = unsigned short;

#ifndef M_PI
#  define M_PI 3.14159265358979323846
#endif

#include <type_traits>

template <typename T>
inline constexpr int signum(T x, std::false_type) {
  return T(0) < x;
}

template <typename T>
inline constexpr int signum(T x, std::true_type) {
  return (T(0) < x) - (x < T(0));
}

template <typename T>
inline constexpr int signum(T x) {
  return signum(x, std::is_signed<T>());
}

#endif
