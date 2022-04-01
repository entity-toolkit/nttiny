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
    float phi_max = phi_range[0] + (phi_range[1] - phi_range[0]) * (float)(i + 1) / (float)(resolution);
    auto p01 = ImPlot::PlotToPixels(ImPlotPoint(center[0] + radius * std::sin(phi_min),
                                                center[1] + radius * std::cos(phi_min)));
    auto p02 = ImPlot::PlotToPixels(ImPlotPoint(center[0] + radius * std::sin(phi_max),
                                                center[1] + radius * std::cos(phi_max)));
    ImPlot::GetPlotDrawList()->AddLine(p01, p02, IM_COL32(0, 0, 0, 255), 0.2);
  }
  ImPlot::PopPlotClipRect();
}
} // namespace nttiny

template class nttiny::Data<int>;
template class nttiny::Data<float>;
template class nttiny::Data<double>;