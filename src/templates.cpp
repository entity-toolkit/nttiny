#include "vis.h"
#include "vis.cpp"

#include "api.h"
#include "api.cpp"

#include "plots.h"
#include "plots.cpp"

template class Visualization<int>;
template class Visualization<float>;
template class Visualization<double>;

template class SimulationAPI<int>;
template class SimulationAPI<float>;
template class SimulationAPI<double>;

template class Pcolor2d<int>;
template class Pcolor2d<float>;
template class Pcolor2d<double>;
