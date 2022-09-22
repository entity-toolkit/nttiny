#include "defs.h"
#include "tools.h"

#include <implot.h>
#include <imgui.h>

#include <cmath>

namespace nttiny::tools {
void drawCircle(const ImVec2& center,
                const float& radius,
                const ImVec2& phi_range,
                const int& resolution,
                const ImU32& color) {
  ImPlot::PushPlotClipRect();
  for (int i{0}; i < resolution; ++i) {
    float phi_min = phi_range[0] + (phi_range[1] - phi_range[0]) * (float)(i) / (float)(resolution);
    float phi_max
        = phi_range[0] + (phi_range[1] - phi_range[0]) * (float)(i + 1) / (float)(resolution);
    auto p01 = ImPlot::PlotToPixels(ImPlotPoint(center[0] + radius * std::sin(phi_min),
                                                center[1] + radius * std::cos(phi_min)));
    auto p02 = ImPlot::PlotToPixels(ImPlotPoint(center[0] + radius * std::sin(phi_max),
                                                center[1] + radius * std::cos(phi_max)));
    ImPlot::GetPlotDrawList()->AddLine(p01, p02, color, 0.2);
  }
  ImPlot::PopPlotClipRect();
}
} // namespace nttiny::tools
