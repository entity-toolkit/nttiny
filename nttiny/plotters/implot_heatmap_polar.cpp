#include "implot_utils.h"
#include "implot_heatmap_polar.h"

#include <implot.h>
#include <implot_internal.h>

#include <iostream>

namespace ImPlot {

#ifndef SIGN
#  define SIGN(x) (((x) < 0.0) ? -1.0 : 1.0)
#endif

#ifndef ABS
#  define ABS(x) (((x) < 0.0) ? -(x) : (x))
#endif

#ifndef QLOGSCALE
#  define QLOGSCALE(x) ((SIGN(x) * powf(ABS(x), 0.25f)))
#endif

template <typename T>
struct GetterHeatmapPolarRowMaj {
  GetterHeatmapPolarRowMaj(const T* values,
                           int rows,
                           int cols,
                           double scale_min,
                           double scale_max,
                           const T* r_grid,
                           const T* th_grid,
                           bool use_log)
      : Values(values),
        Count(rows * cols),
        Rows(rows),
        Cols(cols),
        ScaleMin(scale_min),
        ScaleMax(scale_max),
        RGrid(r_grid),
        ThGrid(th_grid),
        UseLog(use_log) {}
  template <typename I>
  IMPLOT_INLINE ArcC operator()(I idx) const {
    const int r = idx / Cols;
    const int c = idx % Cols;
    const int idx1 = (Cols - c - 1) * Rows + r;
    double val = UseLog ? (double)Values[idx1] : (double)(QLOGSCALE(Values[idx1]));
    T rlow{RGrid[r]}, rhigh{RGrid[r + 1]};
    T th1{ThGrid[c]}, th2{ThGrid[c + 1]};
    ArcC arc;
    arc.Min.x = rlow;
    arc.Min.y = th1;
    arc.Max.x = rhigh;
    arc.Max.y = th2;
    const float t = ImClamp((float)ImRemap01(val, ScaleMin, ScaleMax), 0.0f, 1.0f);
    arc.Color = GImPlot->ColormapData.LerpTable(GImPlot->Style.Colormap, t);
    return arc;
  }
  const T* const Values;
  const int Count, Rows, Cols;
  const double ScaleMin, ScaleMax;
  const T *const RGrid, *const ThGrid;
  const bool UseLog;
};

template <typename T>
struct GetterHeatmapPolarColMaj {
  GetterHeatmapPolarColMaj(const T* values,
                           int rows,
                           int cols,
                           double scale_min,
                           double scale_max,
                           const T* r_grid,
                           const T* th_grid,
                           bool use_log)
      : Values(values),
        Count(rows * cols),
        Rows(rows),
        Cols(cols),
        ScaleMin(scale_min),
        ScaleMax(scale_max),
        RGrid(r_grid),
        ThGrid(th_grid),
        UseLog(use_log) {}
  template <typename I>
  IMPLOT_INLINE ArcC operator()(I idx) const {
    const int r = idx % Cols;
    const int c = idx / Cols;
    const int idx1 = (Rows - r - 1) * Cols + c;
    double val = UseLog ? (double)Values[idx1] : (double)(QLOGSCALE(Values[idx1]));
    T rlow{RGrid[r]}, rhigh{RGrid[r + 1]};
    T th1{ThGrid[c]}, th2{ThGrid[c + 1]};
    ArcC arc;
    arc.Min.x = rlow;
    arc.Min.y = th1;
    arc.Max.x = rhigh;
    arc.Max.y = th2;
    const float t = ImClamp((float)ImRemap01(val, ScaleMin, ScaleMax), 0.0f, 1.0f);
    arc.Color = GImPlot->ColormapData.LerpTable(GImPlot->Style.Colormap, t);
    return arc;
  }
  const T* const Values;
  const int Count, Rows, Cols;
  const double ScaleMin, ScaleMax;
  const T *const RGrid, *const ThGrid;
  const bool UseLog;
};

#undef SIGN
#undef ABS
#undef QLOGSCALE

template <typename T>
void RenderHeatmapPolar(ImDrawList&, // draw_list,
                        const T* values,
                        int rows,
                        int cols,
                        double scale_min,
                        double scale_max,
                        const T* r_grid,
                        const T* th_grid,
                        bool col_maj,
                        bool use_log) {
  if (scale_min == 0 && scale_max == 0) {
    T temp_min, temp_max;
    ImMinMaxArray(values, rows * cols, &temp_min, &temp_max);
    scale_min = (double)temp_min;
    scale_max = (double)temp_max;
  }
  if (col_maj) {
    GetterHeatmapPolarColMaj<T> getter(
        values, rows, cols, scale_min, scale_max, r_grid, th_grid, use_log);
    RenderPrimitives1<RendererArcC>(getter);
  } else {
    GetterHeatmapPolarRowMaj<T> getter(
        values, rows, cols, scale_min, scale_max, r_grid, th_grid, use_log);
    RenderPrimitives1<RendererArcC>(getter);
  }
}

template <typename T>
void PlotHeatmapPolar(const char* label_id,
                      const T* values,
                      int rows,
                      int cols,
                      double scale_min,
                      double scale_max,
                      const T* r_grid,
                      const T* th_grid,
                      bool use_log,
                      const ImPlotPoint& bounds_min,
                      const ImPlotPoint& bounds_max,
                      ImPlotHeatmapFlags flags) {
  if (BeginItemEx(label_id, FitterRect(bounds_min, bounds_max))) {
    ImDrawList& draw_list = *GetPlotDrawList();
    const bool col_maj = ImHasFlag(flags, ImPlotHeatmapFlags_ColMajor);
    RenderHeatmapPolar(
        draw_list, values, rows, cols, scale_min, scale_max, r_grid, th_grid, col_maj, use_log);
    EndItem();
  }
}

template IMPLOT_API void PlotHeatmapPolar<float>(const char* label_id,
                                                 const float* values,
                                                 int rows,
                                                 int cols,
                                                 double scale_min,
                                                 double scale_max,
                                                 const float* r_grid,
                                                 const float* th_grid,
                                                 bool,
                                                 const ImPlotPoint& bounds_min,
                                                 const ImPlotPoint& bounds_max,
                                                 ImPlotHeatmapFlags flags);

template IMPLOT_API void PlotHeatmapPolar<double>(const char* label_id,
                                                  const double* values,
                                                  int rows,
                                                  int cols,
                                                  double scale_min,
                                                  double scale_max,
                                                  const double* r_grid,
                                                  const double* th_grid,
                                                  bool,
                                                  const ImPlotPoint& bounds_min,
                                                  const ImPlotPoint& bounds_max,
                                                  ImPlotHeatmapFlags flags);

} // namespace ImPlot