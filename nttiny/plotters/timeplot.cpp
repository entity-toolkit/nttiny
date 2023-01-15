#include "api.h"
#include "defs.h"
#include "metadata.h"
#include "plots.h"

#include <implot/implot.h>

namespace nttiny {

  template <class T, ushort D>
  auto TimePlot<T, D>::draw(ImPlotRect&, UISettings&) -> bool {
    bool  shouldClose = false;
    auto& Sim         = this->m_sim;
    if (ImPlot::BeginPlot("##tplot", ImVec2(-1, -1))) {
      ImPlot::SetupLegend(ImPlotLocation_North | ImPlotLocation_West);
      ImPlot::SetupAxes("time", "", 0, (this->m_autoscale_y) ? ImPlotAxisFlags_AutoFit : 0);
      ImPlot::SetupAxisLimits(
        ImAxis_X1,
        (this->m_roll_x ? std::fmax(Sim->get_time() - this->m_timespan, 0.0f) : 0.0f),
        Sim->get_time(),
        ImGuiCond_Always);
      if (this->m_log_y) {
        ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_Log10);
      }
      for (auto [buffname, buffer] : Sim->buffers) {
        ImPlot::PlotLine(buffname.c_str(),
                         &buffer.Data[0].x,
                         &buffer.Data[0].y,
                         buffer.Data.size(),
                         0,
                         buffer.Offset,
                         2 * sizeof(float));
      }

      for (auto [buffname, buffer] : Sim->buffers) {
        if (ImPlot::BeginLegendPopup(buffname.c_str())) {
          ImGui::PushItemWidth(8 * ImGui::GetFontSize());
          ImGui::BeginGroup();
          {
            ImGui::PushID("tspan");
            ImGui::Checkbox("roll time", &(this->m_roll_x));
            if (this->m_roll_x) {
              ImGui::SameLine();
              float tspan { (float)(this->m_timespan) };
              ImGui::DragFloat(
                "timespan",
                &(tspan),
                0.01f * std::fabs(std::fmax((float)Sim->get_time(), (float)this->m_timespan)),
                0.0f,
                Sim->get_time(),
                "%.3e");
              this->m_timespan = (T)tspan;
            }
            ImGui::Checkbox("autoscale y", &(this->m_autoscale_y));
            ImGui::SameLine();
            ImGui::Checkbox("log y", &(this->m_log_y));
            ImGui::PopID();
          }
          {
            ImGui::Separator();
            shouldClose = this->close();
          }
          ImGui::EndGroup();
          ImPlot::EndLegendPopup();
        }
      }

      ImPlot::EndPlot();
    }
    return shouldClose;
  }

  template <class T, ushort D>
  auto TimePlot<T, D>::exportMetadata() -> PlotMetadata* {
    auto metadata             = new TimeplotMetadata<T> { this->m_ID };
    metadata->m_buff_selected = m_buff_selected;
    metadata->m_autoscale_y   = m_autoscale_y;
    metadata->m_roll_x        = m_roll_x;
    metadata->m_log_y         = m_log_y;
    metadata->m_timespan      = m_timespan;
    return metadata;
  }

  template <class T, ushort D>
  void TimePlot<T, D>::importMetadata(const toml::value& metadata) {
    m_buff_selected = toml::find<int>(metadata, "buff_selected");
    m_autoscale_y   = toml::find<bool>(metadata, "autoscale_y");
    m_roll_x        = toml::find<bool>(metadata, "roll_x");
    m_log_y         = toml::find<bool>(metadata, "log_y");
    m_timespan      = toml::find<T>(metadata, "timespan");
  }

}    // namespace nttiny

template struct nttiny::TimePlot<float, 1>;
template struct nttiny::TimePlot<float, 2>;
template struct nttiny::TimePlot<float, 3>;
template struct nttiny::TimePlot<double, 1>;
template struct nttiny::TimePlot<double, 2>;
template struct nttiny::TimePlot<double, 3>;