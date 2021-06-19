#ifndef SIM_H
#define SIM_H

class Simulation {
public:
  Simulation(int sx, int sy);
  ~Simulation();
  auto get_sx() const -> int { return m_sx; }
  auto get_sy() const -> int { return m_sy; }
  auto get_data1() const -> float * { return m_data1; }
  auto get_data2() const -> float * { return m_data2; }
  void setData();
  void updateData();
private:
  int m_sx, m_sy;
  float *m_data1, *m_data2;
};

#endif
