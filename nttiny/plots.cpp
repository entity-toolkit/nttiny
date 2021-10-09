#include "defs.h"
#include "plots.h"
#include "api.h"

#include <plog/Log.h>
#include <implot.h>
#include <toml.hpp>

#include <cmath>
#include <string>

#define BELYASH_PINK (ImVec4(1.0f,0.745f,0.745f,1))
#define ALISA_BLUE (ImVec4(0.588f,0.784f,0.98f,1))

namespace nttiny {

template <class T>
Ax<T>::Ax(int id) : m_ID(id) {}
template <class T>
Ax<T>::~Ax() = default;
template <class T>
auto Ax<T>::draw() -> bool { return false; }
template <class T>
auto Ax<T>::getId() -> int {
  return -1;
}
template <class T>
void Ax<T>::bindSimulation(SimulationAPI<T>* sim) {
  this->m_sim = sim;
}

template <class T>
auto Plot2d<T>::close() -> bool {
  if (ImGui::Button("X")) {
    return true;
  } else {
    return false;
  }
}

template <class T>
void Plot2d<T>::scale() {
  ImGui::SetNextItemWidth(120);
  if (ImGui::InputFloat("scale", &this->m_scale, 0.01f, 10.0f, "%.3f")) {
    PLOGV_(VISPLOGID) << "Scale changed to " << this->m_scale << ".";
  }
}

template <class T>
auto Pcolor2d<T>::draw() -> bool {
  bool close;
  float plot_size = this->m_plot_size * this->m_scale;
  float cmap_h = this->m_cmap_h * this->m_scale;
  // TODO: this shall come from simulation
  auto x1min = this->m_sim->get_x1min(), x1max = this->m_sim->get_x1max();
  auto x2min = this->m_sim->get_x2min(), x2max = this->m_sim->get_x2max();
  auto aspect = (x2max - x2min) / (x1max - x1min);
  ImGui::Begin(("Pcolor2d [" + std::to_string(this->m_ID) + "]").c_str());
  {
    this->scale();
    ImGui::SameLine(ImGui::GetWindowWidth() - 30);
    close = this->close();
  }
  // Choose field component to display
  std::string field_selected;
  {
    ImGui::Text("Field component to plot:");
    const char ** field_names;
    field_names = new const char*[this->m_sim->fields.size()];
    int i{0};
    for (const auto& fld : this->m_sim->fields) {
      field_names[i] = fld.first.c_str();
      ++i;
    }
    if (ImGui::Combo("", &this->m_field_selected, field_names, this->m_sim->fields.size())) {
      PLOGV_(VISPLOGID) << "Pcolor2d field changed to " << field_names[this->m_field_selected] << ".";
    }
    field_selected = static_cast<std::string>(field_names[this->m_field_selected]);
  }
  // setup axes
  int dim = this->m_sim->fields[field_selected]->get_dimension();
  if (dim != 2) {
    PLOGE_(VISPLOGID) << "Attempting to plot " << dim << "D data as a 2D heatmap.";
  }
  ImPlot::PushColormap(this->m_cmap);
  // TODO: add log colormap here
  if (ImPlot::BeginPlot(
          "", nullptr, nullptr, ImVec2(plot_size, plot_size * aspect), ImPlotFlags_Equal)) {
    // plot
    ImPlot::PlotHeatmap("",
                        this->m_sim->fields[field_selected]->get_data(),
                        this->m_sim->fields[field_selected]->get_size(0),
                        this->m_sim->fields[field_selected]->get_size(1),
                        this->m_vmin,
                        this->m_vmax,
                        nullptr,
                        {x1min, x2min},
                        {x1max, x2max});
    ImPlot::EndPlot();
  }
  // TODO: add close button here
  // decorations
  ImGui::SameLine();
  ImGui::PushItemWidth(m_sidebar_w);
  ImGui::BeginGroup();
  {
    if (ImPlot::ColormapButton(
            ImPlot::GetColormapName(this->m_cmap), ImVec2(this->m_sidebar_w, 0), this->m_cmap)) {
      this->m_cmap = (this->m_cmap + 1) % ImPlot::GetColormapCount();
      PLOGV_(VISPLOGID) << "Changed colormap to " << ImPlot::GetColormapName(this->m_cmap) << ".";
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
      PLOGV_(VISPLOGID) << "Reseting vmin & vmax for Pcolor2d.";
      auto minmax = findMinMax(this->m_sim->fields[field_selected]->get_data(),
                               this->m_sim->fields[field_selected]->get_size(0)
                                   * this->m_sim->fields[field_selected]->get_size(1));
      this->m_vmin = minmax.first;
      this->m_vmax = minmax.second;
    }
  }
  ImGui::EndGroup();
  ImGui::PopItemWidth();
  ImPlot::PopColormap();
  ImGui::End();
  return close;
}

template <class T>
auto Scatter2d<T>::draw() -> bool {
  bool close{false};
  float plot_size = this->m_plot_size * this->m_scale;
  auto x1min = this->m_sim->get_x1min(), x1max = this->m_sim->get_x1max();
  auto x2min = this->m_sim->get_x2min(), x2max = this->m_sim->get_x2max();
  auto aspect = (x2max - x2min) / (x1max - x1min);

  ImGui::Begin(("Scatter2d [" + std::to_string(this->m_ID) + "]").c_str());
  {
    this->scale();
    ImGui::SameLine(ImGui::GetWindowWidth() - 40);
    close = this->close();
  }
  // Choose particles to display
  std::size_t nspec {this->m_sim->particles.size()};
  {
    if ((this->m_prtl_enabled == nullptr) && (nspec != 0)) {
      this->m_prtl_names = new const char*[nspec];
      this->m_prtl_enabled = new bool[nspec];
      std::size_t i{0};
      for (const auto& prtl : this->m_sim->particles) {
        this->m_prtl_enabled[i] = true;
        this->m_prtl_names[i] = prtl.first.c_str();
        ++i;
      }
    } else {
      ImGui::BeginGroup();
      for (std::size_t i{0}; i < nspec; ++i) {
        ImGui::Checkbox(this->m_prtl_names[i], &(this->m_prtl_enabled[i]));
        if (i < nspec - 1) { ImGui::SameLine(); }
      }
      ImGui::EndGroup();
    }
  }
  // display scatter plots
  {
    ImPlot::SetNextPlotLimits(x1min, x1max, x2min, x2max);
    if (ImPlot::BeginPlot("", nullptr, nullptr, ImVec2(plot_size, plot_size * aspect), ImPlotFlags_Equal)) {
      ImVec2 rmin = ImPlot::PlotToPixels(ImPlotPoint(x1min, x2min));
      ImVec2 rmax = ImPlot::PlotToPixels(ImPlotPoint(x1max, x2max));
      ImPlot::PushPlotClipRect();
      ImPlot::GetPlotDrawList()->AddRect(rmin, rmax, IM_COL32(250,250,240,255));
      ImPlot::PopPlotClipRect();

      for (std::size_t i{0}; i < nspec; ++i) {
        if (this->m_prtl_enabled[i]) {
          auto spec {this->m_prtl_names[i]};
          auto npart {this->m_sim->particles[spec].first->get_size(0)};
          if (i == 0) {
            ImPlot::SetNextMarkerStyle(IMPLOT_AUTO, IMPLOT_AUTO, ALISA_BLUE, IMPLOT_AUTO, ALISA_BLUE);
          } else if (i == 1) {
            ImPlot::SetNextMarkerStyle(IMPLOT_AUTO, IMPLOT_AUTO, BELYASH_PINK, IMPLOT_AUTO, BELYASH_PINK);
          }
          ImPlot::PlotScatter(spec,
                              this->m_sim->particles[spec].first->get_data(),
                              this->m_sim->particles[spec].second->get_data(), npart);
        }
      }
    }
    ImPlot::EndPlot();
  }
  ImGui::End();
  return close;
}

template <class T>
auto Pcolor2d<T>::exportMetadata() -> PlotMetadata {
  PlotMetadata metadata;
  metadata.m_ID = this->m_ID;
  metadata.m_type = "Pcolor2d";
  metadata.m_log = m_log;
  metadata.m_vmin = m_vmin;
  metadata.m_vmax = m_vmax;
  metadata.m_cmap = ImPlot::GetColormapName(m_cmap);
  metadata.m_field_selected = m_field_selected;
  return metadata;
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
} // namespace nttiny
