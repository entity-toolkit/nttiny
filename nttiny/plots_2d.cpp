#include "defs.h"
#include "plots_2d.h"
#include "api.h"

#include "implot_heatmap_cart.h"
#include "implot_heatmap_polar.h"

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
auto Pcolor2d<T>::draw() -> bool {
  bool CLOSE{false};
  auto& Sim = this->m_sim;
  auto& Grid = this->m_sim->m_global_grid;
  const auto coord = Grid.m_coord;

  const auto ngh = Grid.m_ngh;
  const auto sx1 = Grid.m_size[0];
  const auto sx2 = Grid.m_size[1];
  auto dx1 = Grid.m_xi[0][1] - Grid.m_xi[0][0];
  auto x1min = Grid.m_xi[0][0] - ngh * dx1;
  auto x1max = Grid.m_xi[0][sx1] + ngh * dx1;
  auto dx2 = Grid.m_xi[1][1] - Grid.m_xi[1][0];
  auto x2min = Grid.m_xi[1][0] - ngh * dx2;
  auto x2max = Grid.m_xi[1][sx2] + ngh * dx2;

  ImPlot::PushColormap(this->m_cmap);
  if (ImPlot::BeginPlot("##", ImVec2(-1, -1), ImPlotFlags_Equal)) {
    if (coord == Coord::Spherical) {
      //   auto x1_grid = Grid.m_xi[0];
      //   auto x2_grid = Grid.m_xi[1];
      //   x1min = 0.0;
      //   x1max = Grid.m_xi[0][sx1 - ngh];
      //   x2min = -x1max;
      //   x2max = x1max;
      //   ImPlot::PlotPolarHeatmap("##",
      //                            Sim->fields[field_selected],
      //                            sx1,
      //                            sx2,
      //                            this->m_vmin,
      //                            this->m_vmax,
      //                            x1_grid,
      //                            x2_grid,
      //                            this->m_log,
      //                            nullptr,
      //                            {x1min, x2min},
      //                            {x1max, x2max});
    } else {
      // template IMPLOT_API void PlotHeatmapCart<float>(const char* label_id,
      //                                                 const float* values,
      //                                                 int rows,
      //                                                 int cols,
      //                                                 double scale_min,
      //                                                 double scale_max,
      //                                                 const char* fmt,
      //                                                 const ImPlotPoint& bounds_min,
      //                                                 const ImPlotPoint& bounds_max,
      //                                                 ImPlotHeatmapFlags flags);

      ImPlot::PlotHeatmapCart("##",
                              Sim->get_selected_field(this->m_field_selected),
                              sx2 + 2 * ngh,
                              sx1 + 2 * ngh,
                              (double)(this->m_vmin),
                              (double)(this->m_vmax),
                              this->m_log,
                              nullptr,
                              ImPlotPoint(x1min, x2min),
                              ImPlotPoint(x1max, x2max),
                              ImPlotAxisFlags_NoGridLines);
    }
    this->outlineDomain();
    Sim->customAnnotatePcolor2d();
    ImPlot::EndPlot();
  }
  if (ImGui::BeginPopupContextItem()) {
    ImGui::PushItemWidth(65);
    ImGui::BeginGroup();
    {
      {
        /* ----------------------------- field selector ----------------------------- */
        ImGui::PushID("fld");
        ImGui::Combo("##", &this->m_field_selected, Sim->get_field_names(), Sim->fields.size());
        ImGui::PopID();
      }

      /* ---------------------------- colormap selector --------------------------- */
      if (ImPlot::ColormapButton(ImPlot::GetColormapName(this->m_cmap),
                                 ImVec2(5.0f * ImGui::GetFontSize(), 0),
                                 this->m_cmap)) {
        this->m_cmap = (this->m_cmap + 1) % ImPlot::GetColormapCount();
      }

      float vmin = std::min(this->m_vmin, this->m_vmax);
      float vmax = std::max(this->m_vmin, this->m_vmax);
      if (vmin == vmax) {
        // rescale if values too close
        vmax = vmin + 1e-10;
      }

      {
        /* -------------------------------- colorbar -------------------------------- */
        ImGui::PushID("vmax");
        ImGui::DragFloat("##", &vmax, 0.01f * std::fabs(vmax - vmin), vmin, +FLT_MAX, "%.3e");
        ImGui::PopID();

        ImPlot::ColormapScale("##", vmin, vmax, ImVec2(-1, 20.0f * ImGui::GetFontSize()));

        ImGui::PushID("vmin");
        ImGui::DragFloat("##", &vmin, 0.01f * std::fabs(vmax - vmin), -FLT_MAX, vmax, "%.3e");
        ImGui::PopID();
      }

      // save rescaled values
      this->m_vmax = (T)vmax;
      this->m_vmin = (T)vmin;

      ImGui::Checkbox("log", &this->m_log);
      // if (ImGui::Button(ICON_FA_ARROWS_LEFT_RIGHT_TO_LINE)) {
      //   PLOGV_(VISPLOGID) << "Reseting vmin & vmax for Pcolor2d.";
      //   auto n_elements{sx1 * sx2};
      //   auto minmax = findMinMax(this->m_sim->fields[field_selected], n_elements, this->m_log);
      //   this->m_vmin = minmax.first;
      //   this->m_vmax = minmax.second;
      //   if (this->m_vmin * this->m_vmax < 0) {
      //     auto max = std::max(std::abs(this->m_vmax), std::abs(this->m_vmin));
      //     this->m_vmin = -max;
      //     this->m_vmax = max;
      //   }
      // }
      CLOSE = this->close();
    }
    ImGui::EndGroup();
    ImGui::PopItemWidth();
    ImGui::EndPopup();
  }
  ImPlot::PopColormap();

  return CLOSE;
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
