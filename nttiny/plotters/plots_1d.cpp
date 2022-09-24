#include "defs.h"
#include "api.h"
#include "plots_1d.h"

#include <implot/implot.h>

namespace nttiny {

template <class T, ushort D>
auto TimePlot<T, D>::draw(ImPlotRect&, UISettings&) -> bool {}

template <class T, ushort D>
auto TimePlot<T, D>::exportMetadata() -> PlotMetadata {
  PlotMetadata metadata;
  metadata.m_ID = this->m_ID;
  metadata.m_type = "TimePlot";
  return metadata;
}

template <class T, ushort D>
void TimePlot<T, D>::importMetadata(const PlotMetadata&) {}

} // namespace nttiny

template struct nttiny::TimePlot<float, 1>;
template struct nttiny::TimePlot<float, 2>;
template struct nttiny::TimePlot<float, 3>;
template struct nttiny::TimePlot<double, 1>;
template struct nttiny::TimePlot<double, 2>;
template struct nttiny::TimePlot<double, 3>;