#ifndef NTTINY_API_H
#define NTTINY_API_H

#include "defs.h"

#include <map>
#include <vector>
#include <array>
#include <string>
#include <utility>

#include <iostream>

namespace nttiny {

enum class Coord { Cartesian, Spherical };

void drawCircle(const point_t& center,
                const float& radius,
                const point_t& phi_range = {0.0f, 2.0f * M_PI},
                const int& resolution = 128);

template <class T, ushort D>
struct Grid {
  Coord m_coord;
  std::array<int, D> m_size;
  ushort m_ngh;
  std::array<T*, D> m_xi;
  Grid(const Coord& coord, const std::array<int, D>& size, const ushort& ngh = 2)
      : m_coord(coord), m_size(size), m_ngh(ngh) {
    for (ushort i{0}; i < D; ++i) {
      m_xi[i] = new T[m_size[i] + 1];
    }
  }
};

template <class T, ushort D>
struct SimulationAPI {
  // ui
  std::map<std::string, T*> fields;
  std::map<std::string, std::array<T*, D>> particles;
  Grid<T, D> m_global_grid;

  SimulationAPI(const Coord& coord, const std::array<int, D>& size, const ushort& ngh = 2)
      : m_global_grid{coord, size, ngh} {}
  ~SimulationAPI() = default;

  auto Index(const int& i, const int& j) const -> int {
    const auto ngh{m_global_grid.m_ngh};
    const auto nx1{m_global_grid.m_size[0] + 2 * ngh};
    const auto nx2{m_global_grid.m_size[1] + 2 * ngh};
    return (i + ngh) + (nx2 - 1 - (j + ngh)) * nx1;
    // const auto ngh{m_global_grid.m_ngh};
    // const auto nx1{m_global_grid.m_size[0] + 2 * ngh};
    // const auto nx2{m_global_grid.m_size[1] + 2 * ngh};
    // return (j + ngh) + (nx1 - 1 - (i + ngh)) * nx2;
  }
  auto Xi(const int& i, const ushort& d) const -> T { return m_global_grid.m_xi[d][i]; }

  // init
  virtual void setData() = 0;

  // updaters
  virtual void stepFwd() = 0;
  virtual void stepBwd() = 0;
  virtual void restart() = 0;
  void updateData() { (!m_paused) ? (m_forward ? stepFwd() : stepBwd()) : void(); }

  // controls
  [[nodiscard]] auto is_paused() const -> bool { return m_paused; }
  [[nodiscard]] auto is_forward() const -> bool { return m_forward; }
  [[nodiscard]] auto get_timestep() const -> int { return m_timestep; }
  [[nodiscard]] auto get_time() const -> T { return m_time; }
  [[nodiscard]] auto get_jumpover() const -> int { return m_jumpover; }
  void set_jumpover(const int& jumpover) { m_jumpover = jumpover; }
  void playToggle() { m_paused = !m_paused; }
  void reverse() { m_forward = !m_forward; }

  // getters
  auto get_field_names() const -> const char** {
    auto field_names = new const char*[fields.size()];
    int i{0};
    for (const auto& fld : fields) {
      field_names[i] = fld.first.c_str();
      ++i;
    }
    return field_names;
  }
  auto get_selected_field(const int& field_selected_int) -> T* {
    auto field_names = get_field_names();
    auto field_selected = static_cast<std::string>(field_names[field_selected_int]);
    return fields[field_selected];
  }

  // additional visuals
  virtual void customAnnotatePcolor2d() = 0;

protected:
  float m_time;
  int m_timestep;
  bool m_paused{true};
  bool m_forward{true};
  int m_jumpover{1};
};
} // namespace nttiny

#endif
