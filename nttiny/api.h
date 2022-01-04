#ifndef NTTINY_API_H
#define NTTINY_API_H

#include "defs.h"

#include <map>
#include <vector>
#include <string>
#include <utility>

namespace nttiny {

template <class T>
struct Data {
  int m_size[2];
  T *m_data;
  double *grid_x1, *grid_x2;

  Data(int nx1, int nx2) : m_size{nx1, nx2} {
    grid_x1 = new double[m_size[0] + 1];
    grid_x2 = new double[m_size[1] + 1];
    allocate(nx1 * nx2);
  }
  ~Data() = default;
  void allocate(std::size_t n) {
    this->m_data = new T[n];
  }
  [[nodiscard]] auto get_size(std::size_t i) const -> int {
    return this->m_size[i];
  }
  [[nodiscard]] auto get_data() const -> T * {
    return this->m_data;
  }
  [[nodiscard]] auto get(std::size_t i, std::size_t j) const -> T {
    return this->m_data[j * this->m_size[0] + i];
  }

  void set_size(std::size_t i, int size) {
    this->m_size[i] = size;
  }
  void set(std::size_t i, std::size_t j, T value) {
    this->m_data[j * this->m_size[0] + i] = value;
  }
};

template <class T> class SimulationAPI {
public:
  // ui
  std::map<std::string, Data<T> *> fields;
  std::map<std::string, std::pair<Data<T> *, Data<T> *>> particles;
  const std::string coords;

  SimulationAPI(const std::string& coords) : coords(coords) {}
  ~SimulationAPI() = default;

  // init
  virtual void setData() = 0;
  [[nodiscard]] auto get_x1min() const -> float { return m_x1x2_extent[0]; }
  [[nodiscard]] auto get_x1max() const -> float { return m_x1x2_extent[1]; }
  [[nodiscard]] auto get_x2min() const -> float { return m_x1x2_extent[2]; }
  [[nodiscard]] auto get_x2max() const -> float { return m_x1x2_extent[3]; }

  // updaters
  virtual void stepFwd() = 0;
  virtual void stepBwd() = 0;
  virtual void restart() = 0;
  void updateData() {
    (!m_paused) ? (m_forward ? stepFwd() : stepBwd()) : void();
  }

  // controls
  [[nodiscard]] auto is_paused() const -> bool { return m_paused; }
  [[nodiscard]] auto is_forward() const -> bool { return m_forward; }
  [[nodiscard]] auto get_timestep() const -> int { return m_timestep; }
  [[nodiscard]] auto get_time() const -> float { return m_time; }
  void playToggle() { m_paused = !m_paused; }
  void reverse() { m_forward = !m_forward; }

protected:
  float m_x1x2_extent[4];
  float m_time;
  int m_timestep;
  bool m_paused {true};
  bool m_forward {true};
};
}

#endif
