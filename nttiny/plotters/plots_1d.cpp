#include "defs.h"
#include "api.h"
#include "plots_1d.h"

#include <implot/implot.h>

namespace nttiny {

template <class T, ushort D>
auto TimePlot<T, D>::draw(ImPlotRect&, UISettings&) -> bool {
  auto& Sim = this->m_sim;
  if (ImPlot::BeginPlot("##tplot", ImVec2(-1, -1))) {
    ImPlot::SetupLegend(ImPlotLocation_North | ImPlotLocation_West);
    ImPlot::SetupAxes("time", "", 0, (this->m_autoscale_y) ? ImPlotAxisFlags_AutoFit : 0);
    ImPlot::SetupAxisLimits(
        ImAxis_X1,
        (this->m_roll_x ? std::fmax(Sim->get_time() - this->m_timespan, 0.0f) : 0.0f),
        Sim->get_time(),
        ImGuiCond_Always);
    if (this->m_log_y) { ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_Log10); }
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
            float tspan{(float)(this->m_timespan)};
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
          if (this->close()) { return true; }
        }
        ImGui::EndGroup();
        ImPlot::EndLegendPopup();
      }
    }

    ImPlot::EndPlot();
  }
  return false;
}

template <class T, ushort D>
auto TimePlot<T, D>::exportMetadata() -> PlotMetadata {
  PlotMetadata metadata;
  metadata.m_ID = this->m_ID;
  metadata.m_type = "TimePlot";
  return metadata;
}

template <class T, ushort D>
void TimePlot<T, D>::importMetadata(const PlotMetadata&) {}

} // namespace nttiny

template struct nttiny::TimePlot<float, 1>;
template struct nttiny::TimePlot<float, 2>;
template struct nttiny::TimePlot<float, 3>;
template struct nttiny::TimePlot<double, 1>;
template struct nttiny::TimePlot<double, 2>;
template struct nttiny::TimePlot<double, 3>;