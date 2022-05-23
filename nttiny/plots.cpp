#include "defs.h"
#include "plots.h"
#include "api.h"

#include <plog/Log.h>
#include <implot.h>
#include <toml.hpp>

#include <cmath>
#include <string>
#include <iostream>

namespace nttiny {

template <class T>
auto Plot2d<T>::close() -> bool {
  if (ImGui::Button("X")) {
    return true;
  } else {
    return false;
  }
}

template <class T>
void Plot2d<T>::outlineDomain(std::string field_selected) {
  ImPlot::PushPlotClipRect();
  if ((this->m_sim->coords == "spherical") || (this->m_sim->coords == "qspherical")) {
    float rmin, rmax;
    if (field_selected == "" || this->m_sim->fields[field_selected]->grid_x1 == nullptr) {
      rmin = this->m_sim->m_global_grid.m_x1[2];
      auto nx1 = this->m_sim->m_global_grid.m_size[0];
      rmax = this->m_sim->m_global_grid.m_x1[nx1 - 2];
    } else {
      rmin = this->m_sim->fields[field_selected]->grid_x1[2];
      auto nx1 = this->m_sim->fields[field_selected]->get_size(0);
      rmax = this->m_sim->fields[field_selected]->grid_x1[nx1 - 2];
    }
    auto p1 = ImPlot::PlotToPixels(ImPlotPoint(0, rmin));
    auto p2 = ImPlot::PlotToPixels(ImPlotPoint(0, rmax));
    ImPlot::GetPlotDrawList()->AddLine(p1, p2, IM_COL32(250, 250, 240, 255), 0.2);
    for (int i{0}; i < 100; ++i) {
      float phi_min = M_PI * (float)(i) / (float)(100);
      float phi_max = M_PI * (float)(i + 1) / (float)(100);
      auto p01
          = ImPlot::PlotToPixels(ImPlotPoint(rmin * std::sin(phi_min), rmin * std::cos(phi_min)));
      auto p02
          = ImPlot::PlotToPixels(ImPlotPoint(rmin * std::sin(phi_max), rmin * std::cos(phi_max)));
      ImPlot::GetPlotDrawList()->AddLine(p01, p02, IM_COL32(250, 250, 240, 255), 0.2);
      auto p11
          = ImPlot::PlotToPixels(ImPlotPoint(rmax * std::sin(phi_min), rmax * std::cos(phi_min)));
      auto p12
          = ImPlot::PlotToPixels(ImPlotPoint(rmax * std::sin(phi_max), rmax * std::cos(phi_max)));
      ImPlot::GetPlotDrawList()->AddLine(p11, p12, IM_COL32(250, 250, 240, 255), 0.2);
    }
    auto p3 = ImPlot::PlotToPixels(ImPlotPoint(0, -rmin));
    auto p4 = ImPlot::PlotToPixels(ImPlotPoint(0, -rmax));
    ImPlot::GetPlotDrawList()->AddLine(p3, p4, IM_COL32(250, 250, 240, 255), 0.2);
  } else {
    float xmin, xmax, ymin, ymax;
    if (field_selected == "" || this->m_sim->fields[field_selected]->grid_x1 == nullptr) {
      xmin = this->m_sim->m_global_grid.m_x1[2];
      auto nx = this->m_sim->m_global_grid.m_size[0];
      xmax = this->m_sim->m_global_grid.m_x1[nx - 2];
      ymin = this->m_sim->m_global_grid.m_x2[2];
      auto ny = this->m_sim->m_global_grid.m_size[1];
      ymax = this->m_sim->m_global_grid.m_x2[ny - 2];
    } else {
      xmin = this->m_sim->fields[field_selected]->grid_x1[2];
      auto nx = this->m_sim->fields[field_selected]->get_size(0);
      xmax = this->m_sim->fields[field_selected]->grid_x1[nx - 2];
      ymin = this->m_sim->fields[field_selected]->grid_x2[2];
      auto ny = this->m_sim->fields[field_selected]->get_size(0);
      ymax = this->m_sim->fields[field_selected]->grid_x2[ny - 2];
    }
    ImVec2 rmin = ImPlot::PlotToPixels(ImPlotPoint(xmin, ymin));
    ImVec2 rmax = ImPlot::PlotToPixels(ImPlotPoint(ymax, xmax));
    ImPlot::PushPlotClipRect();
    ImPlot::GetPlotDrawList()->AddRect(rmin, rmax, IM_COL32(250, 250, 240, 255));
    ImPlot::PopPlotClipRect();

    // add cartesian here
  }
  ImPlot::PopPlotClipRect();
}

template <class T>
void Plot2d<T>::scale() {
  ImGui::SetNextItemWidth(ImGui::GetFontSize() * 6.0f);
  if (ImGui::SliderFloat("scale", &this->m_scale, 0.01f, 10.0f, "%.3f")) {
    PLOGV_(VISPLOGID) << "Scale changed to " << this->m_scale << ".";
  }
}

template <class T>
auto Pcolor2d<T>::draw() -> bool {
  bool close;
  float plot_size = this->m_plot_size * this->m_scale;
  float cmap_h = this->m_cmap_h * this->m_scale;
  auto x1min = this->m_sim->get_x1min(), x1max = this->m_sim->get_x1max();
  auto x2min = this->m_sim->get_x2min(), x2max = this->m_sim->get_x2max();
  float aspect;
  if ((this->m_sim->coords == "spherical") || (this->m_sim->coords == "qspherical")) {
    aspect = 1.0f;
    plot_size *= 1.75f;
  } else {
    aspect = (x2max - x2min) / (x1max - x1min);
  }
  ImGui::Begin(("Pcolor2d [" + std::to_string(this->m_ID) + "]").c_str());
  {
    this->scale();
    ImGui::SameLine(ImGui::GetWindowWidth() - 3.0f * ImGui::GetFontSize());
    close = this->close();
  }
  // Choose field component to display
  std::string field_selected;
  {
    ImGui::Text("Field to plot:");
    const char** field_names;
    field_names = new const char*[this->m_sim->fields.size()];
    int i{0};
    for (const auto& fld : this->m_sim->fields) {
      field_names[i] = fld.first.c_str();
      ++i;
    }
    if (ImGui::Combo("", &this->m_field_selected, field_names, this->m_sim->fields.size())) {
      PLOGV_(VISPLOGID) << "Pcolor2d field changed to " << field_names[this->m_field_selected]
                        << ".";
    }
    field_selected = static_cast<std::string>(field_names[this->m_field_selected]);
  }
  // setup axes
  ImPlot::PushColormap(this->m_cmap);

  if (ImPlot::BeginPlot("", ImVec2(plot_size, plot_size * aspect), ImPlotFlags_Equal)) {
    // if (ImPlot::BeginPlot("", ImVec2(-1, plot_size), ImPlotFlags_Equal)) {
    if ((this->m_sim->coords == "spherical") || (this->m_sim->coords == "qspherical")) {
      double *x1_grid, *x2_grid;
      x1min = 0.0;
      if (field_selected == "" || this->m_sim->fields[field_selected]->grid_x1 == nullptr) {
        x1_grid = this->m_sim->m_global_grid.m_x1;
        x2_grid = this->m_sim->m_global_grid.m_x2;
        auto nx1 = this->m_sim->m_global_grid.m_size[0];
        x1max = this->m_sim->m_global_grid.m_x1[nx1 - 2];
      } else {
        x1_grid = this->m_sim->fields[field_selected]->grid_x1;
        x2_grid = this->m_sim->fields[field_selected]->grid_x2;
        x1max = this->m_sim->fields[field_selected]
                    ->grid_x1[this->m_sim->fields[field_selected]->get_size(0)];
      }
      x2min = -x1max;
      x2max = x1max;
      ImPlot::PlotPolarHeatmap("",
                               this->m_sim->fields[field_selected]->get_data(),
                               this->m_sim->fields[field_selected]->get_size(1),
                               this->m_sim->fields[field_selected]->get_size(0),
                               this->m_vmin,
                               this->m_vmax,
                               x1_grid,
                               x2_grid,
                               this->m_log,
                               nullptr,
                               {x1min, x2min},
                               {x1max, x2max});
    } else {
      ImPlot::PlotHeatmap("",
                          this->m_sim->fields[field_selected]->get_data(),
                          this->m_sim->fields[field_selected]->get_size(1),
                          this->m_sim->fields[field_selected]->get_size(0),
                          this->m_vmin,
                          this->m_vmax,
                          nullptr,
                          {x1min, x2min},
                          {x1max, x2max});
    }
    this->outlineDomain(field_selected);
    this->m_sim->customAnnotatePcolor2d();
    ImPlot::EndPlot();
  }

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

    ImGui::PushID(0);
    ImGui::InputFloat("", &this->m_vmax, 0.0f, 1000.0f, "%.1e");
    ImGui::PopID();

    ImPlot::ColormapScale("", vmin, vmax, ImVec2(this->m_sidebar_w, cmap_h));

    ImGui::PushID(1);
    ImGui::InputFloat("", &this->m_vmin, 0.0f, 1000.0f, "%.1e");
    ImGui::PopID();

    ImGui::Checkbox("log", &this->m_log);
    if (ImGui::Button("reset")) {
      PLOGV_(VISPLOGID) << "Reseting vmin & vmax for Pcolor2d.";
      auto n_elements{this->m_sim->fields[field_selected]->get_size(0)
                      * this->m_sim->fields[field_selected]->get_size(1)};
      auto minmax
          = findMinMax(this->m_sim->fields[field_selected]->get_data(), n_elements, this->m_log);
      this->m_vmin = minmax.first;
      this->m_vmax = minmax.second;
      if (this->m_vmin * this->m_vmax < 0) {
        auto max = std::max(std::abs(this->m_vmax), std::abs(this->m_vmin));
        this->m_vmin = -max;
        this->m_vmax = max;
      }
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
  float aspect;
  if ((this->m_sim->coords == "spherical") || (this->m_sim->coords == "qspherical")) {
    aspect = 1.0f;
    plot_size *= 1.75f;
  } else {
    aspect = (x2max - x2min) / (x1max - x1min);
  }

  ImGui::Begin(("Scatter2d [" + std::to_string(this->m_ID) + "]").c_str());
  {
    this->scale();
    ImGui::SameLine(ImGui::GetWindowWidth() - 40);
    close = this->close();
  }

  // Choose particles to display
  std::size_t nspec{this->m_sim->particles.size()};
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
    if ((this->m_sim->coords == "spherical") || (this->m_sim->coords == "qspherical")) {
      // x1min = 0.0;
      // x1max = this->m_sim->fields["Ex1"]->grid_x1[this->m_sim->fields["Ex1"]->get_size(0)];
      // x2min = -x1max;
      // x2max = x1max;
      // ImPlot::SetNextAxesLimits(x1min, x1max, x2min, x2max, true);
    }
    if (ImPlot::BeginPlot("", ImVec2(plot_size, plot_size * aspect), ImPlotFlags_Equal)) {
      for (std::size_t i{0}; i < nspec; ++i) {
        if (this->m_prtl_enabled[i]) {
          auto spec{this->m_prtl_names[i]};
          auto npart{this->m_sim->particles[spec].first->get_size(0)};
          if (i == 1) {
            ImPlot::SetNextMarkerStyle(
                IMPLOT_AUTO, IMPLOT_AUTO, BELYASH_PINK, IMPLOT_AUTO, BELYASH_PINK);
          }
          ImPlot::PlotScatter(spec,
                              this->m_sim->particles[spec].first->get_data(),
                              this->m_sim->particles[spec].second->get_data(),
                              npart);
        }
      }
      // TODO: fix this
      this->outlineDomain();
      ImPlot::EndPlot();
    }
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

template <class T>
void Pcolor2d<T>::importMetadata(const PlotMetadata& metadata) {
  m_log = metadata.m_log;
  m_vmin = metadata.m_vmin;
  m_vmax = metadata.m_vmax;
  m_cmap = ImPlot::GetColormapIndex(metadata.m_cmap.c_str());
  m_field_selected = metadata.m_field_selected;
}

template <class T>
auto Scatter2d<T>::exportMetadata() -> PlotMetadata {
  PlotMetadata metadata;
  metadata.m_ID = this->m_ID;
  metadata.m_type = "Scatter2d";
  return metadata;
}

template <class T>
void Scatter2d<T>::importMetadata(const PlotMetadata&) {}

} // namespace nttiny

template class nttiny::Pcolor2d<int>;
template class nttiny::Pcolor2d<float>;
template class nttiny::Pcolor2d<double>;

template class nttiny::Scatter2d<int>;
template class nttiny::Scatter2d<float>;
template class nttiny::Scatter2d<double>;

// ! LEGACY code
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
