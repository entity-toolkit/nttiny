#ifndef SIM_H
#define SIM_H

class Simulation {
public:
  Simulation(int sx, int sy, int steps_per_second);
  ~Simulation();

  void setData();
  auto get_sx() const -> int { return m_sx; }
  auto get_sy() const -> int { return m_sy; }

  void updateData() { (!m_paused) ? (m_forward ? stepFwd() : stepBwd()) : void(); }
  void stepFwd();
  void stepBwd();
  auto get_data1() const -> float * { return m_data1; }
  auto get_data2() const -> float * { return m_data2; }

  auto is_paused() const -> bool { return m_paused; }
  auto is_forward() const -> bool { return m_forward; }
  auto get_timestep() const -> int { return m_timestep; }
  auto get_steps_per_second() const -> int { return m_steps_per_second; }
  void set_steps_per_second(int steps_per_second) { m_steps_per_second = steps_per_second; }
  void playToggle() { m_paused = !m_paused; }
  void reverse() { m_forward = !m_forward; }
private:
  int m_timestep;
  int m_sx, m_sy;
  float *m_data1, *m_data2;
  bool m_paused = false;
  bool m_forward = true;
  int m_steps_per_second;
};

#endif
