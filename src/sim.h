#ifndef SIM_H
#define SIM_H

class Simulation {
public:
  Simulation(int sx, int sy, int steps_per_second);
  ~Simulation() = default;
  bool updated;

  // ui
  const char* fields[2];
  int field_selected;

  // init
  void setData();
  [[nodiscard]] auto get_sx() const -> int { return m_sx; }
  [[nodiscard]] auto get_sy() const -> int { return m_sy; }

  // updaters
  virtual void stepFwd() = 0;
  virtual void stepBwd() = 0;
  void updateData() { (!m_paused) ? (m_forward ? stepFwd() : stepBwd()) : void(); }
  [[nodiscard]] auto get_data1() const -> float * { return m_data1; }
  [[nodiscard]] auto get_data2() const -> float * { return m_data2; }

  // controls
  [[nodiscard]] auto is_paused() const -> bool { return m_paused; }
  [[nodiscard]] auto is_forward() const -> bool { return m_forward; }
  [[nodiscard]] auto get_timestep() const -> int { return m_timestep; }
  [[nodiscard]] auto get_steps_per_second() const -> int { return m_steps_per_second; }
  void set_steps_per_second(int steps_per_second) { m_steps_per_second = steps_per_second; }
  void playToggle() { m_paused = !m_paused; }
  void reverse() { m_forward = !m_forward; }
protected:
  int m_timestep;
  int m_sx, m_sy;
  float *m_data1, *m_data2;
  bool m_paused = false;
  bool m_forward = true;
  int m_steps_per_second;
};

class FakeSimulation : public Simulation {
public:
  FakeSimulation(int sx, int sy, int steps_per_second);
  ~FakeSimulation();
  void stepFwd() override;
  void stepBwd() override;
};

#endif
