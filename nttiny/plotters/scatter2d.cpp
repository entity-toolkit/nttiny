#include "api.h"
#include "defs.h"
#include "icons.h"
#include "implot_heatmap_cart.h"
#include "implot_heatmap_polar.h"
#include "metadata.h"
#include "plots.h"

#include <implot/implot.h>
#include <plog/Log.h>
#include <toml11/toml.hpp>

#include <cmath>
#include <string>
#include <type_traits>

namespace nttiny {

  template <class T>
  auto Scatter2d<T>::draw(ImPlotRect& shared_axes, UISettings& ui_settings) -> bool {
    auto&      Sim   = this->m_sim;
    auto&      Grid  = this->m_sim->m_global_grid;
    const auto coord = Grid.m_coord;

    const auto ngh   = Grid.m_ngh;
    const auto sx1   = Grid.m_size[0];
    const auto sx2   = Grid.m_size[1];
    auto       dx1   = Grid.m_xi[0][1] - Grid.m_xi[0][0];
    auto       x1min = Grid.m_xi[0][0] - ngh * dx1;
    auto       x1max = Grid.m_xi[0][sx1] + ngh * dx1;
    auto       dx2   = Grid.m_xi[1][1] - Grid.m_xi[1][0];
    auto       x2min = Grid.m_xi[1][0] - ngh * dx2;
    auto       x2max = Grid.m_xi[1][sx2] + ngh * dx2;

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
        auto              x1    = species.second.second[0];
        auto              x2    = species.second.second[1];
        ImPlot::PlotScatter(species.first.c_str(), x1, x2, nprtl);
      }
      this->outlineDomain(ui_settings);
      Sim->customAnnotatePcolor2d(ui_settings);
      ImPlot::EndPlot();
    }
    if (ImGui::BeginPopupContextItem("popup")) {
      ImGui::BeginGroup();
      {
        ImGui::Checkbox("link axes", &this->m_share_axes);
        ImGui::Separator();
        if (this->close()) {
          return true;
        }
      }
      ImGui::EndGroup();
      ImGui::EndPopup();
    }
    return false;
  }

  template <class T>
  auto Scatter2d<T>::exportMetadata() -> PlotMetadata* {
    return new Scatter2dMetadata(this->m_ID);
  }

  template <class T>
  void Scatter2d<T>::importMetadata(const toml::value&) {}

}    // namespace nttiny

template class nttiny::Scatter2d<float>;
template class nttiny::Scatter2d<double>;