#include "defs.h"
#include "api.h"

namespace nttiny {
  template <class T>
  SimulationAPI<T>::SimulationAPI(int sx, int sy) : m_sx(sx), m_sy(sy) {
    this->m_x1x2_extent[0] = 0.0f;
    this->m_x1x2_extent[1] = static_cast<float>(sx);
    this->m_x1x2_extent[2] = 0.0f;
    this->m_x1x2_extent[3] = static_cast<float>(sy);
  }
}
