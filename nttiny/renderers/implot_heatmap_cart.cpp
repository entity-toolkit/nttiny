#include "implot_utils.h"
#include "implot_heatmap_cart.h"

#include <implot/implot.h>
#include <implot/implot_internal.h>

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
struct GetterHeatmapCartRowMaj {
  GetterHeatmapCartRowMaj(const T* values,
                          int rows,
                          int cols,
                          double scale_min,
                          double scale_max,
                          double width,
                          double height,
                          double xref,
                          double yref,
                          double ydir,
                          bool use_log)
      : Values(values),
        Count(rows * cols),
        Rows(rows),
        Cols(cols),
        ScaleMin(scale_min),
        ScaleMax(scale_max),
        Width(width),
        Height(height),
        XRef(xref),
        YRef(yref),
        YDir(ydir),
        HalfSize(Width * 0.5, Height * 0.5),
        UseLog(use_log) {}
  template <typename I>
  IMPLOT_INLINE RectC operator()(I idx) const {
    double val = !UseLog ? (double)Values[idx] : (double)(QLOGSCALE(Values[idx]));
    const int r = idx / Cols;
    const int c = idx % Cols;
    const ImPlotPoint p(XRef + HalfSize.x + c * Width, YRef + YDir * (HalfSize.y + r * Height));
    RectC rect;
    rect.Pos = p;
    rect.HalfSize = HalfSize;
    const float t
        = (val > ScaleMax)
            ? 1.0f
            : ((val <= ScaleMin) ? 0.0f
                                 : ImClamp((float)ImRemap01(val, ScaleMin, ScaleMax), 0.0f, 1.0f));
    rect.Color = GImPlot->ColormapData.LerpTable(GImPlot->Style.Colormap, t);
    return rect;
  }
  const T* const Values;
  const int Count, Rows, Cols;
  const double ScaleMin, ScaleMax, Width, Height, XRef, YRef, YDir;
  const ImPlotPoint HalfSize;
  const bool UseLog;
};

template <typename T>
struct GetterHeatmapCartColMaj {
  GetterHeatmapCartColMaj(const T* values,
                          int rows,
                          int cols,
                          double scale_min,
                          double scale_max,
                          double width,
                          double height,
                          double xref,
                          double yref,
                          double ydir,
                          bool use_log)
      : Values(values),
        Count(rows * cols),
        Rows(rows),
        Cols(cols),
        ScaleMin(scale_min),
        ScaleMax(scale_max),
        Width(width),
        Height(height),
        XRef(xref),
        YRef(yref),
        YDir(ydir),
        HalfSize(Width * 0.5, Height * 0.5),
        UseLog(use_log) {}
  template <typename I>
  IMPLOT_INLINE RectC operator()(I idx) const {
    double val = !UseLog ? (double)Values[idx] : (double)(QLOGSCALE(Values[idx]));
    const int r = idx % Cols;
    const int c = idx / Cols;
    const ImPlotPoint p(XRef + HalfSize.x + c * Width, YRef + YDir * (HalfSize.y + r * Height));
    RectC rect;
    rect.Pos = p;
    rect.HalfSize = HalfSize;
    const float t
        = (val > ScaleMax)
            ? 1.0f
            : ((val <= ScaleMin) ? 0.0f
                                 : ImClamp((float)ImRemap01(val, ScaleMin, ScaleMax), 0.0f, 1.0f));
    rect.Color = GImPlot->ColormapData.LerpTable(GImPlot->Style.Colormap, t);
    return rect;
  }
  const T* const Values;
  const int Count, Rows, Cols;
  const double ScaleMin, ScaleMax, Width, Height, XRef, YRef, YDir;
  const ImPlotPoint HalfSize;
  const bool UseLog;
};

#undef SIGN
#undef ABS
#undef QLOGSCALE

template <typename T>
void RenderHeatmapCart(ImDrawList& draw_list,
                       const T* values,
                       int rows,
                       int cols,
                       double scale_min,
                       double scale_max,
                       const ImPlotPoint& bounds_min,
                       const ImPlotPoint& bounds_max,
                       bool reverse_y,
                       bool col_maj,
                       bool use_log) {
  ImPlotContext& gp = *GImPlot;
  Transformer2 transformer;
  if (scale_min == 0 && scale_max == 0) {
    T temp_min, temp_max;
    ImMinMaxArray(values, rows * cols, &temp_min, &temp_max);
    scale_min = (double)temp_min;
    scale_max = (double)temp_max;
  }
  if (scale_min == scale_max) {
    ImVec2 a = transformer(bounds_min);
    ImVec2 b = transformer(bounds_max);
    ImU32 col = GetColormapColorU32(0, gp.Style.Colormap);
    draw_list.AddRectFilled(a, b, col);
    return;
  }
  const double yref = reverse_y ? bounds_max.y : bounds_min.y;
  const double ydir = reverse_y ? -1 : 1;
  if (col_maj) {
    GetterHeatmapCartColMaj<T> getter(values,
                                      rows,
                                      cols,
                                      scale_min,
                                      scale_max,
                                      (bounds_max.x - bounds_min.x) / cols,
                                      (bounds_max.y - bounds_min.y) / rows,
                                      bounds_min.x,
                                      yref,
                                      ydir,
                                      use_log);
    RenderPrimitives1<RendererRectC>(getter);
  } else {
    GetterHeatmapCartRowMaj<T> getter(values,
                                      rows,
                                      cols,
                                      scale_min,
                                      scale_max,
                                      (bounds_max.x - bounds_min.x) / cols,
                                      (bounds_max.y - bounds_min.y) / rows,
                                      bounds_min.x,
                                      yref,
                                      ydir,
                                      use_log);
    RenderPrimitives1<RendererRectC>(getter);
  }
}

template <typename T>
void PlotHeatmapCart(const char* label_id,
                     const T* values,
                     int rows,
                     int cols,
                     double scale_min,
                     double scale_max,
                     bool use_log,
                     const ImPlotPoint& bounds_min,
                     const ImPlotPoint& bounds_max,
                     ImPlotHeatmapFlags flags) {
  if (BeginItemEx(label_id, FitterRect(bounds_min, bounds_max))) {
    ImDrawList& draw_list = *GetPlotDrawList();
    const bool col_maj = ImHasFlag(flags, ImPlotHeatmapFlags_ColMajor);
    RenderHeatmapCart(draw_list,
                      values,
                      rows,
                      cols,
                      scale_min,
                      scale_max,
                      bounds_min,
                      bounds_max,
                      true,
                      col_maj,
                      use_log);
    EndItem();
  }
}

template IMPLOT_API void PlotHeatmapCart<float>(const char* label_id,
                                                const float* values,
                                                int rows,
                                                int cols,
                                                double scale_min,
                                                double scale_max,
                                                bool,
                                                const ImPlotPoint& bounds_min,
                                                const ImPlotPoint& bounds_max,
                                                ImPlotHeatmapFlags flags);

template IMPLOT_API void PlotHeatmapCart<double>(const char* label_id,
                                                 const double* values,
                                                 int rows,
                                                 int cols,
                                                 double scale_min,
                                                 double scale_max,
                                                 bool,
                                                 const ImPlotPoint& bounds_min,
                                                 const ImPlotPoint& bounds_max,
                                                 ImPlotHeatmapFlags flags);

} // namespace ImPlot