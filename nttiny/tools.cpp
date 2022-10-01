#include "defs.h"
#include "tools.h"

#include <implot/implot.h>
#include <imgui/imgui.h>

#include <iomanip>
#include <sstream>
#include <string>
#include <cmath>

namespace nttiny::tools {
void drawCircle(const ImVec2& center,
                const float& radius,
                const ImVec2& phi_range,
                const int& resolution,
                const ImVec4& color) {
  ImPlot::PushPlotClipRect();
  auto col = ImGui::ColorConvertFloat4ToU32(color);
  for (int i{0}; i < resolution; ++i) {
    float phi_min = phi_range[0] + (phi_range[1] - phi_range[0]) * (float)(i) / (float)(resolution);
    float phi_max
        = phi_range[0] + (phi_range[1] - phi_range[0]) * (float)(i + 1) / (float)(resolution);
    auto p01 = ImPlot::PlotToPixels(ImPlotPoint(center[0] + radius * std::sin(phi_min),
                                                center[1] + radius * std::cos(phi_min)));
    auto p02 = ImPlot::PlotToPixels(ImPlotPoint(center[0] + radius * std::sin(phi_max),
                                                center[1] + radius * std::cos(phi_max)));

    ImPlot::GetPlotDrawList()->AddLine(p01, p02, col, 0.2);
  }
  ImPlot::PopPlotClipRect();
}

std::string zeroPadLeft(std::string const& str, int n, char paddedChar) {
  std::ostringstream ss;
  ss << std::right << std::setfill(paddedChar) << std::setw(n) << str;
  return ss.str();
}

} // namespace nttiny::tools
