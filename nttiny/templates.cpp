#include "vis.h"
#include "vis.cpp"

#include "plots.h"
#include "plots.cpp"

template class nttiny::Visualization<int>;
template class nttiny::Visualization<float>;
template class nttiny::Visualization<double>;

template class nttiny::Data<int>;
template class nttiny::Data<float>;
template class nttiny::Data<double>;

template class nttiny::Pcolor2d<int>;
template class nttiny::Pcolor2d<float>;
template class nttiny::Pcolor2d<double>;

template class nttiny::Scatter2d<int>;
template class nttiny::Scatter2d<float>;
template class nttiny::Scatter2d<double>;
