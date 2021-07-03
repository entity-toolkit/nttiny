#include "defs.h"
#include "plots.h"
#include "api.h"

#include <fmt/core.h>
#include <implot.h>

#include <cmath>
#include <string>

template <class T> Ax<T>::Ax(int id) : m_ID(id) {}
template <class T> Ax<T>::~Ax() = default;
template <class T> void Ax<T>::draw() {}
template <class T> auto Ax<T>::getId() -> int { return -1; }
template <class T> void Ax<T>::bindSimulation(SimulationAPI<T> *sim) {
  this->m_sim = sim;
}

template <class T> void Plot2d<T>::scale() {
  ImGui::SetNextItemWidth(120);
  ImGui::InputFloat("scale", &this->m_scale, 0.01f, 10.0f, "%.3f");
}

template <class T> void Pcolor2d<T>::draw() {
  float plot_size = this->m_plot_size * this->m_scale;
  float cmap_h = this->m_cmap_h * this->m_scale;
  // TODO: this shall come from simulation
  auto x1min = this->m_sim->get_x1min(), x1max = this->m_sim->get_x1max();
  auto x2min = this->m_sim->get_x2min(), x2max = this->m_sim->get_x2max();
  auto aspect = (x2max - x2min) / (x1max - x1min);
  ImGui::Begin(fmt::format("Pcolor2d [{}]", this->m_ID).c_str());
  this->scale();
  // Choose field component to display
  std::string field_selected;
  {
    ImGui::Text("Field component to plot:");
    const char *field_names[2];
    int i{0};
    for (const auto &fld : this->m_sim->fields) {
      field_names[i] = fld.first.c_str();
      ++i;
    }
    ImGui::Combo("", &this->m_field_selected, field_names,
                 IM_ARRAYSIZE(field_names));
    field_selected =
        static_cast<std::string>(field_names[this->m_field_selected]);
  }
  // setup axes
  int dim = this->m_sim->fields[field_selected]->get_dimension();
  if (dim != 2) {
    ImGui::TextColored(ImVec4(1.0f, 0.39f, 0.28f, 1.0f),
                       "WARNING: Attempting to plot %dD data as a 2D heatmap",
                       dim);
  }
  ImPlot::PushColormap(this->m_cmap);
  // TODO: add log colormap here
  if (ImPlot::BeginPlot("", nullptr, nullptr,
                        ImVec2(plot_size, plot_size * aspect),
                        ImPlotFlags_Equal)) {
    // plot
    ImPlot::PlotHeatmap("", this->m_sim->fields[field_selected]->get_data(),
                        this->m_sim->fields[field_selected]->get_size(0),
                        this->m_sim->fields[field_selected]->get_size(1),
                        this->m_vmin, this->m_vmax, nullptr, {x1min, x2min},
                        {x1max, x2max});
    ImPlot::EndPlot();
  }
  // decorations
  ImGui::SameLine();
  ImGui::PushItemWidth(m_sidebar_w);
  ImGui::BeginGroup();
  {
    if (ImPlot::ColormapButton(ImPlot::GetColormapName(this->m_cmap),
                               ImVec2(this->m_sidebar_w, 0), this->m_cmap)) {
      this->m_cmap = (this->m_cmap + 1) % ImPlot::GetColormapCount();
    }
    float vmin, vmax;
    vmin = std::min(this->m_vmin, this->m_vmax);
    vmax = std::max(this->m_vmin, this->m_vmax);
    if (vmin == vmax) { // hack
      vmax = vmin + 0.00001f;
    }
    this->m_vmin = vmin;
    this->m_vmax = vmax;
    ImGui::InputFloat("max", &this->m_vmax, 0.0f, 1000.0f, "%.3f");
    ImPlot::ColormapScale("", vmin, vmax, ImVec2(this->m_sidebar_w, cmap_h));
    ImGui::InputFloat("min", &this->m_vmin, 0.0f, 1000.0f, "%.3f");
    ImGui::Checkbox("log", &this->m_log);
    if (ImGui::Button("reset")) {
      auto minmax =
          findMinMax(this->m_sim->fields[field_selected]->get_data(),
                     this->m_sim->fields[field_selected]->get_size(0) *
                         this->m_sim->fields[field_selected]->get_size(1));
      this->m_vmin = minmax.first;
      this->m_vmax = minmax.second;
    }
  }
  ImGui::EndGroup();
  ImGui::PopItemWidth();
  ImPlot::PopColormap();
  ImGui::End();
}

// template <typename T>
// void Plot::draw(T *x_values, T *y_values, int n, const std::string &label) {
//   float plot_size = m_plot_size * m_scale;
//   ImGui::Begin(fmt::format("Plot [{}]", m_ID).c_str());
//   m_xlabel = "x";
//   m_ylabel = "y";
//   scale();
//   if (ImPlot::BeginPlot("Line Plot", m_xlabel.c_str(), m_ylabel.c_str(),
//                         ImVec2(plot_size * m_ratio, plot_size))) {
//     ImPlot::PlotLine(label.c_str(), x_values, y_values, n);
//     ImPlot::EndPlot();
//   }
//   ImGui::End();
// }
//
// template void Plot::draw<int>(int *x_values, int *y_values, int n,
//                               const std::string &label = std::string());
// template void Plot::draw<float>(float *x_values, float *y_values, int n,
//                                 const std::string &label = std::string());
// template void Plot::draw<double>(double *x_values, double *y_values, int n,
//                                  const std::string &label = std::string());
