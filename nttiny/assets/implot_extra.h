#ifndef IMPLOT_EXTRA_H
#define IMPLOT_EXTRA_H

#include "implot_utils.h"

#include <implot.h>
#include <implot_internal.h>

namespace ImPlot {

struct ArcInfo {
  ImPlotPoint Min, Max;
  ImU32 Color;
};

template <typename T>
void PlotPolarHeatmap(const char*,
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