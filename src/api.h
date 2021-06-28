#ifndef NTTINY_API_H
#define NTTINY_API_H

#include <map>
#include <vector>
#include <string>

template<class T>
class SimulationAPI {
public:
  SimulationAPI(int sx, int sy);
  ~SimulationAPI() = default;

  // ui
  int field_selected;
  std::map<std::string, T*> fields;

  // init
  virtual void setData() = 0;
  [[nodiscard]] auto get_sx() const -> int { return m_sx; }
  [[nodiscard]] auto get_sy() const -> int { return m_sy; }

  // updaters
  virtual void stepFwd() = 0;
  virtual void stepBwd() = 0;
  void updateData() { (!m_paused) ? (m_forward ? stepFwd() : stepBwd()) : void(); }

  // controls
  [[nodiscard]] auto is_paused() const -> bool { return m_paused; }
  [[nodiscard]] auto is_forward() const -> bool { return m_forward; }
  [[nodiscard]] auto get_timestep() const -> int { return m_timestep; }
  void playToggle() { m_paused = !m_paused; }
  void reverse() { m_forward = !m_forward; }
protected:
  int m_timestep;
  int m_sx, m_sy;
  bool m_paused = false;
  bool m_forward = true;
};

#endif
