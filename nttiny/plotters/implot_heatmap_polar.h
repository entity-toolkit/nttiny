#ifndef IMPLOT_HEATMAP_POLAR_H
#define IMPLOT_HEATMAP_POLAR_H

#include "implot_utils.h"

#include <implot.h>
#include <implot_internal.h>

namespace ImPlot {

struct ArcInfo {
  ImPlotPoint Min, Max;
  ImU32 Color;
};

template <typename T>
void PlotHeatmapPolar(const char*,
                      const T*,
                      int,
                      int,
                      T,
                      T,
                      T*,
                      T*,
                      bool,
                      const char*,
                      const ImPlotPoint&,
                      const ImPlotPoint&);

} // namespace ImPlot

#endif