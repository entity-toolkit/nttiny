#include "api.h"

template struct nttiny::Grid<float, 1>;
template struct nttiny::Grid<float, 2>;
template struct nttiny::Grid<float, 3>;

template struct nttiny::Grid<double, 1>;
template struct nttiny::Grid<double, 2>;
template struct nttiny::Grid<double, 3>;

template struct nttiny::SimulationAPI<float, 1>;
template struct nttiny::SimulationAPI<float, 2>;
template struct nttiny::SimulationAPI<float, 3>;

template struct nttiny::SimulationAPI<double, 1>;
template struct nttiny::SimulationAPI<double, 2>;
template struct nttiny::SimulationAPI<double, 3>;