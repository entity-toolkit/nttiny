#include "api.h"
#include "defs.h"
#include "icons.h"
#include "metadata.h"
#include "plots.h"

#include <implot/implot.h>
#include <plog/Log.h>
#include <toml11/toml.hpp>

#include <cmath>
#include <iostream>
#include <string>
#include <type_traits>

namespace nttiny {

  template <class T>
  void LinePlot1D<T>::rescaleMinMax() {
    auto minmax  = this->m_sim->get_min_max(this->m_field_selected, this->m_log);
    this->m_ymin = minmax.first;
    this->m_ymax = minmax.second;
    if ((this->m_ymin * this->m_ymax < 0) && this->m_symmetric) {
      auto max     = std::max(std::fabs(this->m_ymax), std::fabs(this->m_ymin));
      this->m_ymin = -max;
      this->m_ymax = max;
    }
  }

  template <class T>
  auto LinePlot1D<T>::draw(ImPlotRect& shared_axes, UISettings& ui_settings) -> bool {
    bool       shouldClose = false;
    auto&      Sim         = this->m_sim;
    auto&      Grid        = this->m_sim->m_global_grid;
    const auto coord       = Grid.m_coord;

    const auto ngh         = Grid.m_ngh;
    const auto sx          = Grid.m_size[0];

    if (ImPlot::BeginPlot("##lplot", ImVec2(-1, -1))) {
      if (this->m_log) {
        ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_Log10);
      }
      ImPlot::SetupLegend(ImPlotLocation_North | ImPlotLocation_West);
      ImPlot::SetupAxisLinks(ImAxis_X1,
                             this->m_share_axes ? &shared_axes.X.Min : NULL,
                             this->m_share_axes ? &shared_axes.X.Max : NULL);
      ImPlot::PlotLine(Sim->get_field_names()[this->m_field_selected],
                       Grid.m_xi_c[0],
                       Sim->get_selected_field(this->m_field_selected),
                       sx);

      if (ImPlot::BeginLegendPopup(Sim->get_field_names()[this->m_field_selected])) {
        ImGui::PushItemWidth(-1);
        {
          /* ----------------------------- field selector ----------------------------- */
          ImGui::PushID("fld");
          ImGui::Combo(
            "##", &this->m_field_selected, Sim->get_field_names(), Sim->fields.size());
          ImGui::PopID();
        }
        ImGui::PopItemWidth();

        ImGui::PushItemWidth(8 * ImGui::GetFontSize());
        ImGui::BeginGroup();
        {
          ImGui::PushID("tspan");
          bool symmetric = this->m_symmetric, log = this->m_log, autoscale = this->m_autoscale;
          ImGui::Checkbox("symmetric", &symmetric);
          ImGui::Checkbox("log", &log);
          ImGui::SameLine();
          if (ImGui::Button(ICON_FA_ARROWS_LEFT_RIGHT_TO_LINE)) {
            this->rescaleMinMax();
          }
          ImGui::Checkbox("link axes", &this->m_share_axes);

          ImGui::Checkbox("autoscale", &autoscale);
          if ((autoscale != this->m_autoscale) || (symmetric != this->m_symmetric)
              || (this->m_log != log)) {
            this->m_autoscale   = autoscale;
            this->m_symmetric   = symmetric;
            this->m_log         = log;
            Sim->m_data_changed = true;
          }
          ImGui::PopID();
        }
        {
          ImGui::Separator();
          shouldClose = this->close();
        }
        ImGui::PopItemWidth();
        ImGui::EndGroup();
        ImPlot::EndLegendPopup();
      }

      ImPlot::EndPlot();
    }
    return shouldClose;
  }

  template <class T>
  auto LinePlot1D<T>::exportMetadata() -> PlotMetadata* {
    auto metadata              = new Lineplot1dMetadata { this->m_ID };
    metadata->m_log            = m_log;
    metadata->m_autoscale      = m_autoscale;
    metadata->m_symmetric      = m_symmetric;
    metadata->m_xmin           = m_xmin;
    metadata->m_xmax           = m_xmax;
    metadata->m_ymin           = m_ymin;
    metadata->m_ymax           = m_ymax;
    metadata->m_color          = m_color;
    metadata->m_field_selected = m_field_selected;
    return metadata;
  }

  template <class T>
  void LinePlot1D<T>::importMetadata(const toml::value& metadata) {
    m_log            = toml::find<bool>(metadata, "log");
    m_autoscale      = toml::find<bool>(metadata, "autoscale");
    m_symmetric      = toml::find<bool>(metadata, "symmetric");
    m_xmin           = toml::find<T>(metadata, "xmin");
    m_xmax           = toml::find<T>(metadata, "xmax");
    m_ymin           = toml::find<T>(metadata, "ymin");
    m_ymax           = toml::find<T>(metadata, "ymax");
    auto color       = toml::find<std::vector<float>>(metadata, "color");
    m_color          = ImVec4(color[0], color[1], color[2], color[3]);
    m_field_selected = toml::find<int>(metadata, "field_selected");
  }

}    // namespace nttiny

template class nttiny::LinePlot1D<float>;
template class nttiny::LinePlot1D<double>;