#include "defs.h"
#include "plots_2d.h"
#include "api.h"

#include "implot_heatmap_cart.h"
#include "implot_heatmap_polar.h"
#include "icons.h"

#include <plog/Log.h>
#include <implot.h>
#include <toml.hpp>

#include <cmath>
#include <string>
#include <type_traits>

namespace nttiny {

template <class T>
auto Plot2d<T>::close(const int& w) -> bool {
  if (ImGui::Button("delete", ImVec2(w, 0))) {
    return true;
  } else {
    return false;
  }
}

template <class T>
void Plot2d<T>::outlineDomain() {
  const auto sx1 = this->m_sim->m_global_grid.m_size[0];
  const auto sx2 = this->m_sim->m_global_grid.m_size[1];
  const auto x1min = this->m_sim->m_global_grid.m_xi[0][0];
  const auto x1max = this->m_sim->m_global_grid.m_xi[0][sx1];
  const auto x2min = this->m_sim->m_global_grid.m_xi[1][0];
  const auto x2max = this->m_sim->m_global_grid.m_xi[1][sx2];
  const auto coord = this->m_sim->m_global_grid.m_coord;

  auto thickness = 2.5f;
  auto color = IM_COL32(255, 255, 255, 255);
  auto thres = 32;

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

template <class T>
void Pcolor2d<T>::rescaleMinMax() {
  auto minmax = this->m_sim->get_min_max(this->m_field_selected, this->m_log);
  this->m_vmin = minmax.first;
  this->m_vmax = minmax.second;
  if (this->m_vmin * this->m_vmax < 0) {
    auto max = std::max(std::fabs(this->m_vmax), std::fabs(this->m_vmin));
    this->m_vmin = -max;
    this->m_vmax = max;
  }
}

template <class T>
auto Pcolor2d<T>::draw(ImPlotRect& shared_axes) -> bool {
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
    ImPlot::SetupAxisLinks(ImAxis_X1,
                           this->m_share_axes ? &shared_axes.X.Min : NULL,
                           this->m_share_axes ? &shared_axes.X.Max : NULL);
    ImPlot::SetupAxisLinks(ImAxis_Y1,
                           this->m_share_axes ? &shared_axes.Y.Min : NULL,
                           this->m_share_axes ? &shared_axes.Y.Max : NULL);
    if (coord == Coord::Spherical) {
      ImPlot::PlotHeatmapPolar("##",
                               Sim->get_selected_field(this->m_field_selected),
                               sx1 + 2 * ngh,
                               sx2 + 2 * ngh,
                               (double)this->m_vmin,
                               (double)this->m_vmax,
                               Grid.m_xi_gh[0],
                               Grid.m_xi_gh[1],
                               this->m_log,
                               ImPlotPoint(0.0, -x1max),
                               ImPlotPoint(x1max, x1max),
                               ImPlotAxisFlags_NoGridLines);
    } else {
      ImPlot::PlotHeatmapCart("##",
                              Sim->get_selected_field(this->m_field_selected),
                              sx2 + 2 * ngh,
                              sx1 + 2 * ngh,
                              (double)(this->m_vmin),
                              (double)(this->m_vmax),
                              this->m_log,
                              ImPlotPoint(x1min, x2min),
                              ImPlotPoint(x1max, x2max),
                              ImPlotAxisFlags_NoGridLines);
    }
    this->outlineDomain();
    Sim->customAnnotatePcolor2d();
    ImPlot::EndPlot();
  }
  if (ImGui::BeginPopupContextItem("popup")) {
    ImGui::PushItemWidth(8 * ImGui::GetFontSize());
    ImGui::BeginGroup();
    {
      ImGui::PushItemWidth(-1);
      {
        /* ----------------------------- field selector ----------------------------- */
        ImGui::PushID("fld");
        ImGui::Combo("##", &this->m_field_selected, Sim->get_field_names(), Sim->fields.size());
        ImGui::PopID();
      }

      /* ---------------------------- colormap selector --------------------------- */
      if (ImPlot::ColormapButton(
              ImPlot::GetColormapName(this->m_cmap), ImVec2(-1, 0), this->m_cmap)) {
        this->m_cmap = (this->m_cmap + 1) % ImPlot::GetColormapCount();
      }

      float vmax = this->m_vmax, vmin = this->m_vmin;

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
      ImGui::PopItemWidth();

      // save rescaled values
      this->m_vmax = (T)vmax;
      this->m_vmin = (T)vmin;

      ImGui::Checkbox("log", &this->m_log);
      ImGui::SameLine();
      if (ImGui::Button(ICON_FA_ARROWS_LEFT_RIGHT_TO_LINE)) { this->rescaleMinMax(); }
      ImGui::Checkbox("link axes", &this->m_share_axes);
      ImGui::Checkbox("autoscale", &this->m_autoscale);
      ImGui::Separator();
      if (this->close()) { return true; }
    }
    ImGui::EndGroup();
    ImGui::PopItemWidth();
    ImGui::EndPopup();
  }
  if (this->m_autoscale && Sim->m_data_changed) { this->rescaleMinMax(); }
  ImPlot::PopColormap();

  return false;
}

template <class T>
auto Scatter2d<T>::draw(ImPlotRect& shared_axes) -> bool {
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

  if (coord == Coord::Spherical) {
    ImPlot::SetNextAxesLimits(0.0f, (float)x1max, -(float)x1max, (float)x1max);
  } else {
    ImPlot::SetNextAxesLimits((float)x1min, (float)x1max, (float)x2min, (float)x2max);
  }
  if (ImPlot::BeginPlot("##", ImVec2(-1, -1), ImPlotFlags_Equal)) {
    ImPlot::SetupAxisLinks(ImAxis_X1,
                           this->m_share_axes ? &shared_axes.X.Min : NULL,
                           this->m_share_axes ? &shared_axes.X.Max : NULL);
    ImPlot::SetupAxisLinks(ImAxis_Y1,
                           this->m_share_axes ? &shared_axes.Y.Min : NULL,
                           this->m_share_axes ? &shared_axes.Y.Max : NULL);
    for (auto species : Sim->particles) {
      const std::size_t nprtl = species.second.first;
      auto x1 = species.second.second[0];
      auto x2 = species.second.second[1];
      ImPlot::PlotScatter(species.first.c_str(), x1, x2, nprtl);
    }
    this->outlineDomain();
    ImPlot::EndPlot();
  }
  if (ImGui::BeginPopupContextItem("popup")) {
    ImGui::BeginGroup();
    {
      ImGui::Checkbox("link axes", &this->m_share_axes);
      ImGui::Separator();
      if (this->close(8 * ImGui::GetFontSize())) { return true; }
    }
    ImGui::EndGroup();
    ImGui::EndPopup();
  }
  return false;
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