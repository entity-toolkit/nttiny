#include "defs.h"
#include "sim.h"

#include <plog/Log.h>

Simulation::Simulation(int sx, int sy) : m_sx(sx), m_sy(sy) {
  m_data1 = new float[sx * sy];
  m_data2 = new float[sx * sy];
}

Simulation::~Simulation() {
  delete[] m_data1;
  delete[] m_data2;
}

void Simulation::setData() {
  float f_sx{static_cast<float>(m_sx)};
  float f_sy{static_cast<float>(m_sy)};
  for (int j{0}; j < m_sy; ++j) {
    float f_j{static_cast<float>(j)};
    for (int i{0}; i < m_sx; ++i) {
      float f_i{static_cast<float>(i)};
      m_data1[j * m_sx + i] = 0.5f * (f_i / f_sx) + 0.5f * (f_j / f_sy);
      m_data2[j * m_sx + i] = (f_i / f_sx) * (f_j / f_sy);
    }
  }
}

void Simulation::updateData() {
  for (int j{0}; j < m_sy; ++j) {
    for (int i{0}; i < m_sx; ++i) {
      m_data1[j * m_sx + i] = m_data1[j * m_sx + i] + 0.00001f;
      m_data2[j * m_sx + i] = m_data2[j * m_sx + i] + 0.00001f;
    }
  }
}
