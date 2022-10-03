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
auto Pcolor2d<T>::draw(ImPlotRect& shared_axes, UISettings& ui_settings) -> bool {
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
    ImPlot::SetupLegend(ImPlotLocation_North | ImPlotLocation_West);
    ImPlot::SetupAxisLinks(ImAxis_X1,
                           this->m_share_axes ? &shared_axes.X.Min : NULL,
                           this->m_share_axes ? &shared_axes.X.Max : NULL);
    ImPlot::SetupAxisLinks(ImAxis_Y1,
                           this->m_share_axes ? &shared_axes.Y.Min : NULL,
                           this->m_share_axes ? &shared_axes.Y.Max : NULL);
    if (coord == Coord::Spherical) {
      ImPlot::PlotHeatmapPolar(Sim->get_field_names()[this->m_field_selected],
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
      ImPlot::PlotHeatmapCart(Sim->get_field_names()[this->m_field_selected],
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
    this->outlineDomain(ui_settings);
    Sim->customAnnotatePcolor2d(ui_settings);

    if (ImPlot::BeginLegendPopup(Sim->get_field_names()[this->m_field_selected])) {
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

        /* ---------------------------- colormap selector --------------------------- */
        {
          ImPlotContext& gp = *GImPlot;
          ImPlot::ColormapIcon(this->m_cmap);
          ImGui::SameLine();
          ImGui::Text(gp.ColormapData.GetName(this->m_cmap), "##");
          ImGui::SameLine();
          if (ImGui::BeginCombo(
                  "##", gp.ColormapData.GetName(gp.Style.Colormap), ImGuiComboFlags_NoPreview)) {
            for (int i = 0; i < gp.ColormapData.Count; ++i) {
              const char* name = gp.ColormapData.GetName(i);
              ImPlot::ColormapIcon(i);
              ImGui::SameLine();
              if (ImGui::Selectable(name, gp.Style.Colormap == i)) {
                gp.Style.Colormap = i;
                ImPlot::BustItemCache();
                this->m_cmap = GImPlot->Style.Colormap;
              }
            }
            ImGui::EndCombo();
          }
        }

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
      ImPlot::EndLegendPopup();
    }

    ImPlot::EndPlot();
  }
  if (this->m_autoscale && Sim->m_data_changed) { this->rescaleMinMax(); }
  ImPlot::PopColormap();

  return false;
}

template <class T>
auto Pcolor2d<T>::exportMetadata() -> PlotMetadata* {
  auto metadata = new Pcolor2dMetadata{this->m_ID};
  metadata->m_log = m_log;
  metadata->m_autoscale = m_autoscale;
  metadata->m_vmin = m_vmin;
  metadata->m_vmax = m_vmax;
  metadata->m_cmap = ImPlot::GetColormapName(m_cmap);
  metadata->m_field_selected = m_field_selected;
  return metadata;
}

template <class T>
void Pcolor2d<T>::importMetadata(const toml::value& metadata) {
  m_log = toml::find<bool>(metadata, "log");
  m_autoscale = toml::find<bool>(metadata, "autoscale");
  m_vmin = toml::find<T>(metadata, "vmin");
  m_vmax = toml::find<T>(metadata, "vmax");
  m_cmap = ImPlot::GetColormapIndex(toml::find<std::string>(metadata, "cmap").c_str());
  m_field_selected = toml::find<int>(metadata, "field_selected");
}

} // namespace nttiny

template class nttiny::Pcolor2d<float>;
template class nttiny::Pcolor2d<double>;