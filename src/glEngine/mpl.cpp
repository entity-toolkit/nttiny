#include "defs.h"
#include "mpl.h"

#include <fmt/core.h>
#include <implot.h>

#include <string>

static int plot_id{0};

Plot2d::Plot2d(ImVec2 xlims, ImVec2 ylims)
    : m_ID(plot_id), m_xlims(xlims), m_ylims(ylims) {
  ++plot_id;
}

void Plot2d::scale() {
  ImGui::SetNextItemWidth(120);
  ImGui::InputFloat("scale", &m_scale, 0.01f, 10.0f, "%.3f");
}

template <typename T> void Pcolor2d::draw(T *values, int sx, int sy) {
  float plot_size = m_plot_size * m_scale;
  float cmap_h = m_cmap_h * m_scale;
  float aspect = static_cast<float>(sy) / static_cast<float>(sx);
  ImGui::Begin(fmt::format("Pcolor2d [{}]", m_ID).c_str());
  scale();
  // setup axes
  ImPlot::PushColormap(m_cmap);
  if (ImPlot::BeginPlot("", nullptr, nullptr,
                        ImVec2(plot_size, plot_size * aspect),
                        ImPlotFlags_Equal)) {
    // plot
    ImPlot::PlotHeatmap("", values, sx, sy, m_vmin, m_vmax, nullptr,
                        {m_xlims.x, m_ylims.x}, {m_xlims.y, m_ylims.y});
    ImPlot::EndPlot();
  }
  // decorations
  ImGui::SameLine();
  ImGui::PushItemWidth(m_sidebar_w);
  ImGui::BeginGroup();
  {
    if (ImPlot::ColormapButton(ImPlot::GetColormapName(m_cmap),
                               ImVec2(m_sidebar_w, 0), m_cmap)) {
      m_cmap = (m_cmap + 1) % ImPlot::GetColormapCount();
    }
    float vmin, vmax;
    vmin = std::min(m_vmin, m_vmax);
    vmax = std::max(m_vmin, m_vmax);
    if (vmin == vmax) { // hack
      vmax = vmin + 0.00001f;
    }
    m_vmin = vmin;
    m_vmax = vmax;
    ImGui::InputFloat("max", &m_vmax, 0.0f, 1000.0f, "%.3f");
    ImPlot::ColormapScale("", vmin, vmax, ImVec2(m_sidebar_w, cmap_h));
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

template <typename T>
void Plot::draw(T *x_values, T *y_values, int n, const std::string &label) {
  float plot_size = m_plot_size * m_scale;
  ImGui::Begin(fmt::format("Plot [{}]", m_ID).c_str());
  m_xlabel = "x";
  m_ylabel = "y";
  scale();
  if (ImPlot::BeginPlot("Line Plot", m_xlabel.c_str(), m_ylabel.c_str(),
                        ImVec2(plot_size * m_ratio, plot_size))) {
    ImPlot::PlotLine(label.c_str(), x_values, y_values, n);
    ImPlot::EndPlot();
  }
  ImGui::End();
}

template void Plot::draw<int>(int *x_values, int *y_values, int n,
                              const std::string &label = std::string());
template void Plot::draw<float>(float *x_values, float *y_values, int n,
                                const std::string &label = std::string());
template void Plot::draw<double>(double *x_values, double *y_values, int n,
                                 const std::string &label = std::string());
