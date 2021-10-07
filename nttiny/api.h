#ifndef NTTINY_API_H
#define NTTINY_API_H

#include <map>
#include <vector>
#include <string>
#include <utility>

namespace nttiny {

  template <class T> class Data {
  protected:
    int m_dimension{0};
    int m_size[3]{0, 0, 0};
    T *m_data;

  public:
    Data() = default;
    ~Data() = default;
    void allocate(std::size_t n) { this->m_data = new T[n]; }
    [[nodiscard]] auto get_dimension() const -> int { return this->m_dimension; }
    [[nodiscard]] auto get_size(std::size_t i) const -> int {
      return this->m_size[i];
    }
    [[nodiscard]] auto get_data() const -> T * { return this->m_data; }
    [[nodiscard]] auto get(std::size_t i) const -> T { return this->m_data[i]; }

    void set_size(std::size_t i, int size) { m_size[i] = size; }
    void set_dimension(int dim) { m_dimension = dim; }
    void set(std::size_t i, T value) { this->m_data[i] = value; }
  };

  template <class T> class SimulationAPI {
  public:
    SimulationAPI(int sx, int sy);
    ~SimulationAPI() = default;

    // ui
    std::map<std::string, Data<T> *> fields;
    std::map<std::string, std::pair<Data<T> *, Data<T> *>> particles;

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
    void playToggle() { m_paused = !m_paused; }
    void reverse() { m_forward = !m_forward; }

  protected:
    int m_sx, m_sy;
    float m_x1x2_extent[4];
    int m_timestep;
    bool m_paused {true};
    bool m_forward {true};
  };
}

#endif
