#include "api.h"

#include "defs.h"

#include <plog/Log.h>
#include <implot.h>

#include <cmath>

namespace nttiny {

void drawCircle(const point_t& center,
                const float& radius,
                const point_t& phi_range,
                const int& resolution) {
  ImPlot::PushPlotClipRect();
  for (int i{0}; i < resolution; ++i) {
    float phi_min = phi_range[0] + (phi_range[1] - phi_range[0]) * (float)(i) / (float)(resolution);
    float phi_max
        = phi_range[0] + (phi_range[1] - phi_range[0]) * (float)(i + 1) / (float)(resolution);
    auto p01 = ImPlot::PlotToPixels(ImPlotPoint(center[0] + radius * std::sin(phi_min),
                                                center[1] + radius * std::cos(phi_min)));
    auto p02 = ImPlot::PlotToPixels(ImPlotPoint(center[0] + radius * std::sin(phi_max),
                                                center[1] + radius * std::cos(phi_max)));
    ImPlot::GetPlotDrawList()->AddLine(p01, p02, IM_COL32(0, 0, 0, 255), 0.2);
  }
  ImPlot::PopPlotClipRect();
}

} // namespace nttiny

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

// template struct nttiny::Data<int>;
// template struct nttiny::Data<float>;
// template struct nttiny::Data<double>;
