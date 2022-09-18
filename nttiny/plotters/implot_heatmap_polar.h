#ifndef IMPLOT_HEATMAP_POLAR_H
#define IMPLOT_HEATMAP_POLAR_H

#include "implot_utils.h"

#include <implot.h>
#include <implot_internal.h>

namespace ImPlot {

template <typename T>
void PlotHeatmapPolar(const char*,
                      const T*,
                      int,
                      int,
                      double,
                      double,
                      const T*,
                      const T*,
                      bool,
                      const ImPlotPoint&,
                      const ImPlotPoint&,
                      ImPlotHeatmapFlags);

} // namespace ImPlot

#endif