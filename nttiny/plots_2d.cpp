#include "defs.h"
#include "plots_2d.h"
#include "api.h"

#include "implot_extra.h"

#include <plog/Log.h>
#include <implot.h>
#include <toml.hpp>

#include <cmath>
#include <string>
#include <iostream>
#include <type_traits>

namespace nttiny {

template <class T>
auto Plot2d<T>::close() -> bool {
  if (ImGui::Button("close")) {
    return true;
  } else {
    return false;
  }
}

template <class T>
void Plot2d<T>::outlineDomain() {
  // const auto nx1 = this->m_sim->m_global_grid.m_size[0];
  // const auto nx2 = this->m_sim->m_global_grid.m_size[1];
  // const auto ngh = this->m_sim->m_global_grid.m_ngh;
  // const auto x1min = this->m_sim->m_global_grid.m_xi[0][ngh];
  // const auto x1max = this->m_sim->m_global_grid.m_xi[0][nx1 - ngh];
  // const auto x2min = this->m_sim->m_global_grid.m_xi[1][ngh];
  // const auto x2max = this->m_sim->m_global_grid.m_xi[1][nx2 - ngh];
  // const auto coord = this->m_sim->m_global_grid.m_coord;

  // ImPlot::PushPlotClipRect();
  // if (coord == Coord::Spherical) {
  //   auto rmin = x1min;
  //   auto rmax = x1max;

  //   auto p1 = ImPlot::PlotToPixels(ImPlotPoint(0, rmin));
  //   auto p2 = ImPlot::PlotToPixels(ImPlotPoint(0, rmax));
  //   ImPlot::GetPlotDrawList()->AddLine(p1, p2, IM_COL32(250, 250, 240, 255), 0.2);
  //   for (int i{0}; i < 100; ++i) {
  //     float phi_min = M_PI * (float)(i) / (float)(100);
  //     float phi_max = M_PI * (float)(i + 1) / (float)(100);
  //     auto p01
  //         = ImPlot::PlotToPixels(ImPlotPoint(rmin * std::sin(phi_min), rmin *
  //         std::cos(phi_min)));
  //     auto p02
  //         = ImPlot::PlotToPixels(ImPlotPoint(rmin * std::sin(phi_max), rmin *
  //         std::cos(phi_max)));
  //     ImPlot::GetPlotDrawList()->AddLine(p01, p02, IM_COL32(250, 250, 240, 255), 0.2);
  //     auto p11
  //         = ImPlot::PlotToPixels(ImPlotPoint(rmax * std::sin(phi_min), rmax *
  //         std::cos(phi_min)));
  //     auto p12
  //         = ImPlot::PlotToPixels(ImPlotPoint(rmax * std::sin(phi_max), rmax *
  //         std::cos(phi_max)));
  //     ImPlot::GetPlotDrawList()->AddLine(p11, p12, IM_COL32(250, 250, 240, 255), 0.2);
  //   }
  //   auto p3 = ImPlot::PlotToPixels(ImPlotPoint(0, -rmin));
  //   auto p4 = ImPlot::PlotToPixels(ImPlotPoint(0, -rmax));
  //   ImPlot::GetPlotDrawList()->AddLine(p3, p4, IM_COL32(250, 250, 240, 255), 0.2);
  // } else {
  //   ImVec2 rmin = ImPlot::PlotToPixels(ImPlotPoint(x1min, x2min));
  //   ImVec2 rmax = ImPlot::PlotToPixels(ImPlotPoint(x2max, x1max));
  //   ImPlot::PushPlotClipRect();
  //   ImPlot::GetPlotDrawList()->AddRect(rmin, rmax, IM_COL32(250, 250, 240, 255));
  //   ImPlot::PopPlotClipRect();

  //   // add cartesian here
  // }
  // ImPlot::PopPlotClipRect();
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
  bool close{false};
  float plot_size = this->m_plot_size * this->m_scale;
  float cmap_h = this->m_cmap_h * this->m_scale;
  const auto nx1 = this->m_sim->m_global_grid.m_size[0];
  const auto nx2 = this->m_sim->m_global_grid.m_size[1];
  const auto ngh = this->m_sim->m_global_grid.m_ngh;
  auto x1min = this->m_sim->m_global_grid.m_xi[0][0];
  auto x1max = this->m_sim->m_global_grid.m_xi[0][nx1];
  auto x2min = this->m_sim->m_global_grid.m_xi[1][0];
  auto x2max = this->m_sim->m_global_grid.m_xi[1][nx2];
  const auto coord = this->m_sim->m_global_grid.m_coord;
  float aspect{1.0f};
  // if (coord == Coord::Spherical) {
  //   aspect = 1.0f;
  //   plot_size *= 1.75f;
  // } else {
  //   aspect = (x2max - x2min) / (x1max - x1min);
  // }
  // if (ImGui::Begin(("Pcolor2d [" + std::to_string(this->m_ID) + "]").c_str())) {

  {
    // this->scale();
    // ImGui::SameLine(ImGui::GetWindowWidth() - 3.0f * ImGui::GetFontSize());
  }

  const auto field_names = new const char*[this->m_sim->fields.size()];
  int i{0};
  for (const auto& fld : this->m_sim->fields) {
    field_names[i] = fld.first.c_str();
    ++i;
  }
  auto field_selected{static_cast<std::string>(field_names[this->m_field_selected])};

  // setup axes
  ImPlot::PushColormap(this->m_cmap);

  if (ImPlot::BeginPlot("##", ImVec2(-1, -1), ImPlotFlags_Equal)) {
    // if (ImPlot::BeginPlot("", ImVec2(-1, plot_size), ImPlotFlags_Equal)) {
    if (coord == Coord::Spherical) {
      auto x1_grid = this->m_sim->m_global_grid.m_xi[0];
      auto x2_grid = this->m_sim->m_global_grid.m_xi[1];
      x1min = 0.0;
      x1max = this->m_sim->m_global_grid.m_xi[0][nx1 - ngh];
      x2min = -x1max;
      x2max = x1max;
      ImPlot::PlotPolarHeatmap("##",
                               this->m_sim->fields[field_selected],
                               nx1,
                               nx2,
                               this->m_vmin,
                               this->m_vmax,
                               x1_grid,
                               x2_grid,
                               this->m_log,
                               nullptr,
                               {x1min, x2min},
                               {x1max, x2max});
    } else {
      ImPlot::PlotHeatmap("##",
                          this->m_sim->fields[field_selected],
                          nx2,
                          nx1,
                          this->m_vmin,
                          this->m_vmax,
                          nullptr,
                          {x1min, x2min},
                          {x1max, x2max});
    }
    this->outlineDomain();
    this->m_sim->customAnnotatePcolor2d();
    ImPlot::EndPlot();
  }
  if (ImGui::BeginPopupContextItem()) {
    ImGui::PushItemWidth(this->m_sidebar_w);
    ImGui::Spacing();
    ImGui::BeginGroup();
    {
      // choose field component to display
      ImGui::Combo("##", &this->m_field_selected, field_names, this->m_sim->fields.size());
      if (ImPlot::ColormapButton(
              ImPlot::GetColormapName(this->m_cmap), ImVec2(this->m_sidebar_w, 0), this->m_cmap)) {
        this->m_cmap = (this->m_cmap + 1) % ImPlot::GetColormapCount();
      }
      float vmin = std::min(this->m_vmin, this->m_vmax);
      float vmax = std::max(this->m_vmin, this->m_vmax);
      if (vmin == vmax) { // hack
        vmax = vmin + 0.00001f;
      }
      ImGui::DragFloatRange2("##", &vmin, &vmax, 0.01f * std::fabs(vmax - vmin), -FLT_MAX, +FLT_MAX, "%.1e");
      this->m_vmax = (T)vmax;
      this->m_vmin = (T)vmin;
      ImPlot::ColormapScale("##", vmin, vmax, ImVec2(this->m_sidebar_w, cmap_h));

      ImGui::Checkbox("log", &this->m_log);
      if (ImGui::Button("reset")) {
        PLOGV_(VISPLOGID) << "Reseting vmin & vmax for Pcolor2d.";
        auto n_elements{nx1 * nx2};
        auto minmax = findMinMax(this->m_sim->fields[field_selected], n_elements, this->m_log);
        this->m_vmin = minmax.first;
        this->m_vmax = minmax.second;
        if (this->m_vmin * this->m_vmax < 0) {
          auto max = std::max(std::abs(this->m_vmax), std::abs(this->m_vmin));
          this->m_vmin = -max;
          this->m_vmax = max;
        }
      }
      close = this->close();
    }
    ImGui::EndGroup();
    ImGui::PopItemWidth();
    ImGui::EndPopup();
  }

  ImPlot::PopColormap();
  return close;
}

template <class T>
auto Scatter2d<T>::draw() -> bool {
  bool close{false};
  // float plot_size = this->m_plot_size * this->m_scale;
  // const auto nx1 = this->m_sim->m_global_grid.m_size[0];
  // const auto nx2 = this->m_sim->m_global_grid.m_size[1];
  // const auto x1min = this->m_sim->m_global_grid.m_xi[0][0];
  // const auto x1max = this->m_sim->m_global_grid.m_xi[0][nx1 - 1];
  // const auto x2min = this->m_sim->m_global_grid.m_xi[1][0];
  // const auto x2max = this->m_sim->m_global_grid.m_xi[1][nx2 - 1];
  // const auto coord = this->m_sim->m_global_grid.m_coord;
  // float aspect;
  // if (coord == Coord::Spherical) {
  //   aspect = 1.0f;
  //   plot_size *= 1.75f;
  // } else {
  //   aspect = (x2max - x2min) / (x1max - x1min);
  // }

  // ImGui::Begin(("Scatter2d [" + std::to_string(this->m_ID) + "]").c_str());
  // {
  //   this->scale();
  //   ImGui::SameLine(ImGui::GetWindowWidth() - 40);
  //   close = this->close();
  // }

  // // Choose particles to display
  // std::size_t nspec{this->m_sim->particles.size()};
  // {
  //   if ((this->m_prtl_enabled == nullptr) && (nspec != 0)) {
  //     this->m_prtl_names = new const char*[nspec];
  //     this->m_prtl_enabled = new bool[nspec];
  //     std::size_t i{0};
  //     for (const auto& prtl : this->m_sim->particles) {
  //       this->m_prtl_enabled[i] = true;
  //       this->m_prtl_names[i] = prtl.first.c_str();
  //       ++i;
  //     }
  //   } else {
  //     ImGui::BeginGroup();
  //     for (std::size_t i{0}; i < nspec; ++i) {
  //       ImGui::Checkbox(this->m_prtl_names[i], &(this->m_prtl_enabled[i]));
  //       if (i < nspec - 1) { ImGui::SameLine(); }
  //     }
  //     ImGui::EndGroup();
  //   }
  // }
  // // // display scatter plots
  // // {
  // //   if (this->m_sim->m_coords == Coord::Spherical) {
  // //     // x1min = 0.0;
  // //     // x1max = this->m_sim->fields["Ex1"]->grid_x1[this->m_sim->fields["Ex1"]->get_size(0)];
  // //     // x2min = -x1max;
  // //     // x2max = x1max;
  // //     // ImPlot::SetNextAxesLimits(x1min, x1max, x2min, x2max, true);
  // //   }
  // //   if (ImPlot::BeginPlot("##", ImVec2(plot_size, plot_size * aspect), ImPlotFlags_Equal)) {
  // //     for (std::size_t i{0}; i < nspec; ++i) {
  // //       if (this->m_prtl_enabled[i]) {
  // //         auto spec{this->m_prtl_names[i]};
  // //         auto npart{this->m_sim->particles[spec].first->get_size(0)};
  // //         if (i == 1) {
  // //           ImPlot::SetNextMarkerStyle(
  // //               IMPLOT_AUTO, IMPLOT_AUTO, BELYASH_PINK, IMPLOT_AUTO, BELYASH_PINK);
  // //         }
  // //         ImPlot::PlotScatter(spec,
  // //                             this->m_sim->particles[spec].first,
  // //                             this->m_sim->particles[spec].second,
  // //                             npart);
  // //       }
  // //     }
  // //     // TODO: fix this
  // //     this->outlineDomain();
  // //     ImPlot::EndPlot();
  // //   }
  // // }
  // ImGui::End();
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

template class nttiny::Pcolor2d<float>;
template class nttiny::Pcolor2d<double>;

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
