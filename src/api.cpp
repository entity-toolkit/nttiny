#include "defs.h"
#include "api.h"

#include <plog/Log.h>

template <class T>
SimulationAPI<T>::SimulationAPI(int sx, int sy) : m_sx(sx), m_sy(sy) {
  this->field_selected = 0;
}
