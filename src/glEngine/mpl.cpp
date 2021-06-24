#include "defs.h"
#include "mpl.h"

#include <fmt/core.h>
#include <implot.h>

static int PLOT_ID{0};

Plot2d::Plot2d(ImVec2 xlims, ImVec2 ylims) : m_ID(PLOT_ID), m_xlims(xlims), m_ylims(ylims) {
  ++PLOT_ID;
}

void Plot2d::scale() {
  ImGui::SetNextItemWidth(120);
  ImGui::InputFloat("scale", &m_scale, 0.01f, 10.0f, "%.3f");
}

template<typename T>
void Pcolor2d::draw(T *values, int sx, int sy) {
  int plot_size_ = m_plot_size * m_scale;
  int cmap_h_ = m_cmap_h * m_scale;
  ImGui::Begin(fmt::format("Pcolor2d [{}]", m_ID).c_str());
  scale();
  ImPlot::PushColormap(m_cmap);
  if (ImPlot::BeginPlot("", NULL, NULL, ImVec2(plot_size_, plot_size_ * sy / sx), ImPlotFlags_Equal)) {
    ImPlot::PlotHeatmap("", values, sx, sy, m_vmin, m_vmax, NULL, m_xlims, m_ylims);
    ImPlot::EndPlot();
  }
  ImGui::SameLine();
  ImGui::PushItemWidth(m_sidebar_w);
  ImGui::BeginGroup();
  {
    if (ImPlot::ColormapButton(ImPlot::GetColormapName(m_cmap), ImVec2(m_sidebar_w, 0), m_cmap)) {
      m_cmap = (m_cmap + 1) % ImPlot::GetColormapCount();
    }
    float vmin, vmax;
    vmin = std::min(m_vmin, m_vmax);
    vmax = std::max(m_vmin, m_vmax);
    if (vmin == vmax) { // hack
      vmax = vmin + 0.00001;
    }
    m_vmin = vmin; m_vmax = vmax;
    ImGui::InputFloat("max", &m_vmax, 0.0f, 1000.0f, "%.3f");
    ImPlot::ColormapScale("", vmin, vmax, ImVec2(m_sidebar_w, cmap_h_));
    ImGui::InputFloat("min", &m_vmin, 0.0f, 1000.0f, "%.3f");
    ImGui::Checkbox("log", &m_log);
    if (ImGui::Button("reset")) {
      auto minmax = findMinMax(values, sx * sy);
      m_vmin = minmax.first;
      m_vmax = minmax.second;
    }
  }
  ImPlot::PopColormap();
  ImGui::PopItemWidth();
  ImGui::EndGroup();
  ImGui::End();
}

template void Pcolor2d::draw<int>(int *values, int sx, int sy);
template void Pcolor2d::draw<float>(float *values, int sx, int sy);
template void Pcolor2d::draw<double>(double *values, int sx, int sy);
