#ifndef IMPLOT_HEATMAP_CART_H
#define IMPLOT_HEATMAP_CART_H

#include "implot_utils.h"

#include <implot/implot.h>
#include <implot/implot_internal.h>

namespace ImPlot {

template <typename T>
void PlotHeatmapCart(const char*,
                     const T*,
                     int,
                     int,
                     double,
                     double,
                     bool,
                     const ImPlotPoint&,
                     const ImPlotPoint&,
                     ImPlotHeatmapFlags);
} // namespace ImPlot

#endif