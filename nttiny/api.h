#ifndef NTTINY_API_H
#define NTTINY_API_H

#include "defs.h"

#include <imgui/imgui.h>

#include <array>
#include <cmath>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace nttiny {

  enum class Coord { Cartesian, Spherical };

  struct UISettings {
    ImVec4 OutlineColor { 1.0f, 1.0f, 1.0f, 1.0f };
  };

  struct ScrollingBuffer {
    int              MaxSize;
    int              Offset;
    ImVector<ImVec2> Data;
    ScrollingBuffer(int max_size = 2000) {
      MaxSize = max_size;
      Offset  = 0;
      Data.reserve(MaxSize);
    }
    void AddPoint(float x, float y) {
      if (Data.size() < MaxSize)
        Data.push_back(ImVec2(x, y));
      else {
        Data[Offset] = ImVec2(x, y);
        Offset       = (Offset + 1) % MaxSize;
      }
    }
    void Erase() {
      if (Data.size() > 0) {
        Data.shrink(0);
        Offset = 0;
      }
    }
  };

  template <class T, ushort D>
  struct Grid {
    Coord              m_coord;
    std::array<int, D> m_size;
    ushort             m_ngh;
    std::array<T*, D>  m_xi;
    std::array<T*, D>  m_xi_gh;

    Grid(const Coord& coord, const std::array<int, D>& size, const ushort& ngh = 2)
      : m_coord(coord), m_size(size), m_ngh(ngh) {
      for (ushort i { 0 }; i < D; ++i) {
        m_xi[i] = new T[m_size[i] + 1];
      }
      if (m_coord == Coord::Spherical) {
        for (ushort i { 0 }; i < D; ++i) {
          m_xi_gh[i] = new T[m_size[i] + 1 + 2 * m_ngh];
        }
      }
    }

    ~Grid() {
      for (ushort i { 0 }; i < D; ++i) {
        delete[] m_xi[i];
      }
      if (m_coord == Coord::Spherical) {
        for (ushort i { 0 }; i < D; ++i) {
          delete[] m_xi_gh[i];
        }
      }
    }

    void ExtendGridWithGhosts() {
      const auto ngh = m_ngh;
      for (ushort d { 0 }; d < D; ++d) {
        auto sx = m_size[d];
        for (int i { 0 }; i <= sx + 2 * ngh; ++i) {
          if (i < ngh) {
            auto dx       = m_xi[d][1] - m_xi[d][0];
            m_xi_gh[d][i] = m_xi[d][0] - (ngh - i) * dx;
          } else if (i >= ngh && i <= sx + ngh) {
            m_xi_gh[d][i] = m_xi[d][i - ngh];
          } else {
            auto dx       = m_xi[d][sx] - m_xi[d][sx - 1];
            m_xi_gh[d][i] = m_xi[d][sx] + (i - sx - ngh) * dx;
          }
        }
      }
    }
  };

  template <class T, ushort D>
  struct SimulationAPI {
    std::map<std::string, T*>                                fields;
    std::map<std::string, std::pair<int, std::array<T*, D>>> particles;
    std::map<std::string, ScrollingBuffer>                   buffers;
    Grid<T, D>                                               m_global_grid;
    bool                                                     m_data_changed { true };

    SimulationAPI(const std::string&        title,
                  const Coord&              coord,
                  const std::array<int, D>& size,
                  const ushort&             ngh = 2)
      : m_title { title }, m_global_grid { coord, size, ngh } {}
    ~SimulationAPI() = default;

    auto Index(const int& i, const int& j) const -> int {
      const auto ngh { m_global_grid.m_ngh };
      const auto nx1 { m_global_grid.m_size[0] + 2 * ngh };
      const auto nx2 { m_global_grid.m_size[1] + 2 * ngh };
      return (i + ngh) + (nx2 - 1 - (j + ngh)) * nx1;
    }
    auto Xi(const int& i, const ushort& d) const -> T {
      return m_global_grid.m_xi[d][i];
    }

    // init
    virtual void setData() = 0;

    // updaters
    virtual void stepFwd() = 0;
    virtual void stepBwd() = 0;
    virtual void restart() = 0;
    void         updateData(const bool& jumpover_set) {
      if (!m_paused) {
        if (m_forward) {
          stepFwd();
        } else {
          stepBwd();
        }
        m_data_changed = true;
      }
      if (jumpover_set && m_data_changed) {
        setData();
      }
    }

    // controls
    [[nodiscard]] auto is_paused() const -> bool {
      return m_paused;
    }
    [[nodiscard]] auto is_forward() const -> bool {
      return m_forward;
    }
    [[nodiscard]] auto get_timestep() const -> int {
      return m_timestep;
    }
    [[nodiscard]] auto get_time() const -> T {
      return m_time;
    }
    [[nodiscard]] auto get_jumpover() const -> int {
      return m_jumpover;
    }
    void set_jumpover(const int& jumpover) {
      m_jumpover = jumpover;
    }
    void playToggle() {
      m_paused = !m_paused;
    }
    void reverse() {
      m_forward = !m_forward;
    }

    // getters
    auto get_field_names() const -> const char** {
      auto field_names = new const char*[fields.size()];
      int  i { 0 };
      for (const auto& fld : fields) {
        field_names[i] = fld.first.c_str();
        ++i;
      }
      return field_names;
    }
    auto get_selected_field(const int& field_selected_int) -> T* {
      auto field_names    = get_field_names();
      auto field_selected = static_cast<std::string>(field_names[field_selected_int]);
      return fields[field_selected];
    }
    auto get_buffer_names() const -> const char** {
      auto buffer_names = new const char*[buffers.size()];
      int  i { 0 };
      for (const auto& buf : buffers) {
        buffer_names[i] = buf.first.c_str();
        ++i;
      }
      return buffer_names;
    }
    auto get_selected_buffer(const int& buffer_selected_int) -> ScrollingBuffer* {
      auto buffer_names    = get_buffer_names();
      auto buffer_selected = static_cast<std::string>(buffer_names[buffer_selected_int]);
      return &buffers[buffer_selected];
    }
    auto get_min_max(const int& field_selected_int, const bool& use_log) -> std::pair<T, T> {
      auto       array = get_selected_field(field_selected_int);
      T          min = (T)(1e20), max = -(T)(1e20);
      const auto sx1 { m_global_grid.m_size[0] };
      const auto sx2 { m_global_grid.m_size[1] };
      for (int j { 0 }; j < sx2; ++j) {
        for (int i { 0 }; i < sx1; ++i) {
          T val = array[Index(i, j)];
          if (use_log) {
            val = (signum(val) * std::pow(std::fabs(val), 0.25f));
          }
          if (i == 0 && j == 0) {
            min = val;
            max = val;
          } else {
            min = std::min(min, val);
            max = std::max(max, val);
          }
        }
      }
      return { min, max };
    }
    auto get_title() const -> std::string {
      return m_title;
    }

    // additional visuals
    virtual void customAnnotatePcolor2d(const UISettings&) = 0;

  protected:
    float             m_time;
    int               m_timestep;
    bool              m_paused { true };
    bool              m_forward { true };
    int               m_jumpover { 1 };
    const std::string m_title;
  };

}    // namespace nttiny

#endif
