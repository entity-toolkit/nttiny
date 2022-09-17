#ifndef IMPLOT_HEATMAP_CART_H
#define IMPLOT_HEATMAP_CART_H

#include "implot_utils.h"

#include <implot.h>
#include <implot_internal.h>

namespace ImPlot {

template <typename T>
void PlotHeatmapCart(const char*,
                     const T*,
                     int,
                     int,
                     T,
                     T,
                     bool,
                     const char*,
                     const ImPlotPoint&,
                     const ImPlotPoint&);

} // namespace ImPlot

#endif