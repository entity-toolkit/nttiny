#include "defs.h"
#include "api.h"
#include "metadata.h"
#include "plots.h"

#include "implot_heatmap_cart.h"
#include "implot_heatmap_polar.h"
#include "icons.h"

#include <plog/Log.h>
#include <implot/implot.h>
#include <toml11/toml.hpp>

#include <cmath>
#include <string>
#include <type_traits>

namespace nttiny {

template <class T>
void Plot2d<T>::outlineDomain(UISettings& ui_settings) {
  const auto sx1 = this->m_sim->m_global_grid.m_size[0];
  const auto sx2 = this->m_sim->m_global_grid.m_size[1];
  const auto x1min = this->m_sim->m_global_grid.m_xi[0][0];
  const auto x1max = this->m_sim->m_global_grid.m_xi[0][sx1];
  const auto x2min = this->m_sim->m_global_grid.m_xi[1][0];
  const auto x2max = this->m_sim->m_global_grid.m_xi[1][sx2];
  const auto coord = this->m_sim->m_global_grid.m_coord;

  auto thickness = 2.5f;
  auto color = ImGui::ColorConvertFloat4ToU32(ui_settings.OutlineColor);
  auto thres = 128;

  ImPlot::PushPlotClipRect();
  if (coord == Coord::Spherical) {
    auto rmin = x1min;
    auto rmax = x1max;
    auto p1 = ImPlot::PlotToPixels(ImPlotPoint(0, rmin));
    auto p2 = ImPlot::PlotToPixels(ImPlotPoint(0, rmax));
    ImPlot::GetPlotDrawList()->AddLine(p1, p2, color, thickness);
    for (int i{0}; i < thres; ++i) {
      float phi_min = M_PI * (float)(i) / (float)(thres);
      float phi_max = M_PI * (float)(i + 1) / (float)(thres);
      auto p01
          = ImPlot::PlotToPixels(ImPlotPoint(rmin * std::sin(phi_min), rmin * std::cos(phi_min)));
      auto p02
          = ImPlot::PlotToPixels(ImPlotPoint(rmin * std::sin(phi_max), rmin * std::cos(phi_max)));
      ImPlot::GetPlotDrawList()->AddLine(p01, p02, color, thickness);
      auto p11
          = ImPlot::PlotToPixels(ImPlotPoint(rmax * std::sin(phi_min), rmax * std::cos(phi_min)));
      auto p12
          = ImPlot::PlotToPixels(ImPlotPoint(rmax * std::sin(phi_max), rmax * std::cos(phi_max)));
      ImPlot::GetPlotDrawList()->AddLine(p11, p12, color, thickness);
    }
    auto p3 = ImPlot::PlotToPixels(ImPlotPoint(0, -rmin));
    auto p4 = ImPlot::PlotToPixels(ImPlotPoint(0, -rmax));
    ImPlot::GetPlotDrawList()->AddLine(p3, p4, color, thickness);
  } else {
    ImVec2 rmin = ImPlot::PlotToPixels(ImPlotPoint(x1min, x2min));
    ImVec2 rmax = ImPlot::PlotToPixels(ImPlotPoint(x1max, x2max));
    ImPlot::GetPlotDrawList()->AddRect(rmin, rmax, color, 0.0f, 0, thickness);
  }
  ImPlot::PopPlotClipRect();
}

} // namespace nttiny

template struct nttiny::Plot2d<float>;
template struct nttiny::Plot2d<double>;