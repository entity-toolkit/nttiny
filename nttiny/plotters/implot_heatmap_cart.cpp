#include "implot_utils.h"
#include "implot_heatmap_cart.h"

#include <implot.h>
#include <implot_internal.h>

namespace ImPlot {

struct RectInfo {
  ImPlotPoint Min, Max;
  ImU32 Color;
};

template <typename TGetter, typename TTransformer>
struct RectRenderer {
  IMPLOT_INLINE RectRenderer(const TGetter& getter, const TTransformer& transformer)
      : Getter(getter), Transformer(transformer), Prims(Getter.Count) {}
  IMPLOT_INLINE bool
  operator()(ImDrawList& DrawList, const ImRect& cull_rect, const ImVec2& uv, int prim) const {
    RectInfo rect = Getter(prim);
    ImVec2 P1 = Transformer(rect.Min);
    ImVec2 P2 = Transformer(rect.Max);

    if ((rect.Color & IM_COL32_A_MASK) == 0
        || !cull_rect.Overlaps(ImRect(ImMin(P1, P2), ImMax(P1, P2))))
      return false;

    DrawList._VtxWritePtr[0].pos = P1;
    DrawList._VtxWritePtr[0].uv = uv;
    DrawList._VtxWritePtr[0].col = rect.Color;
    DrawList._VtxWritePtr[1].pos.x = P1.x;
    DrawList._VtxWritePtr[1].pos.y = P2.y;
    DrawList._VtxWritePtr[1].uv = uv;
    DrawList._VtxWritePtr[1].col = rect.Color;
    DrawList._VtxWritePtr[2].pos = P2;
    DrawList._VtxWritePtr[2].uv = uv;
    DrawList._VtxWritePtr[2].col = rect.Color;
    DrawList._VtxWritePtr[3].pos.x = P2.x;
    DrawList._VtxWritePtr[3].pos.y = P1.y;
    DrawList._VtxWritePtr[3].uv = uv;
    DrawList._VtxWritePtr[3].col = rect.Color;
    DrawList._VtxWritePtr += 4;
    DrawList._IdxWritePtr[0] = (ImDrawIdx)(DrawList._VtxCurrentIdx);
    DrawList._IdxWritePtr[1] = (ImDrawIdx)(DrawList._VtxCurrentIdx + 1);
    DrawList._IdxWritePtr[2] = (ImDrawIdx)(DrawList._VtxCurrentIdx + 3);
    DrawList._IdxWritePtr[3] = (ImDrawIdx)(DrawList._VtxCurrentIdx + 1);
    DrawList._IdxWritePtr[4] = (ImDrawIdx)(DrawList._VtxCurrentIdx + 2);
    DrawList._IdxWritePtr[5] = (ImDrawIdx)(DrawList._VtxCurrentIdx + 3);
    DrawList._IdxWritePtr += 6;
    DrawList._VtxCurrentIdx += 4;
    return true;
  }
  const TGetter& Getter;
  const TTransformer& Transformer;
  const int Prims;
  static const int IdxConsumed = 6;
  static const int VtxConsumed = 4;
};

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
struct GetterHeatmapCart {
  GetterHeatmapCart(const T* values,
                    int rows,
                    int cols,
                    T scale_min,
                    T scale_max,
                    T width,
                    T height,
                    T xref,
                    T yref,
                    T ydir,
                    const bool use_log_scale)
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
        UseLogScale(use_log_scale) {}

  template <typename I>
  IMPLOT_INLINE RectInfo operator()(I idx) const {
    T val;
    if (UseLogScale) {
      val = (T)(QLOGSCALE(Values[idx]));
    } else {
      val = (T)Values[idx];
    }
    const int r = idx / Cols;
    const int c = idx % Cols;
    const ImPlotPoint p(XRef + HalfSize.x + c * Width, YRef + YDir * (HalfSize.y + r * Height));
    RectInfo rect;
    rect.Min.x = p.x - HalfSize.x;
    rect.Min.y = p.y - HalfSize.y;
    rect.Max.x = p.x + HalfSize.x;
    rect.Max.y = p.y + HalfSize.y;
    const float t = ImClamp(ImRemap01((float)val, (float)ScaleMin, (float)ScaleMax), 0.0f, 1.0f);
    rect.Color = GImPlot->ColormapData.LerpTable(GImPlot->Style.Colormap, t);
    return rect;
  }
  const T* const Values;
  const int Count, Rows, Cols;
  const T ScaleMin, ScaleMax, Width, Height, XRef, YRef, YDir;
  const ImPlotPoint HalfSize;
  const bool UseLogScale;
};

#undef SIGN
#undef ABS
#undef QLOGSCALE

template <typename T, typename Transformer>
void RenderHeatmapCart(Transformer transformer,
                       ImDrawList& DrawList,
                       const T* values,
                       int rows,
                       int cols,
                       T scale_min,
                       T scale_max,
                       bool use_log_scale,
                       const char* fmt,
                       const ImPlotPoint& bounds_min,
                       const ImPlotPoint& bounds_max,
                       bool reverse_y) {
  ImPlotContext& gp = *GImPlot;
  if (scale_min == 0 && scale_max == 0) {
    T temp_min, temp_max;
    ImMinMaxArray(values, rows * cols, &temp_min, &temp_max);
    scale_min = (T)temp_min;
    scale_max = (T)temp_max;
  }
  if (scale_min == scale_max) {
    ImVec2 a = transformer(bounds_min);
    ImVec2 b = transformer(bounds_max);
    ImU32 col = GetColormapColorU32(0, gp.Style.Colormap);
    DrawList.AddRectFilled(a, b, col);
    return;
  }
  const double yref = reverse_y ? bounds_max.y : bounds_min.y;
  const double ydir = reverse_y ? -1 : 1;
  GetterHeatmapCart<T> getter(values,
                              rows,
                              cols,
                              scale_min,
                              scale_max,
                              (bounds_max.x - bounds_min.x) / cols,
                              (bounds_max.y - bounds_min.y) / rows,
                              bounds_min.x,
                              yref,
                              ydir,
                              use_log_scale);
  switch (GetCurrentScale()) {
  case ImPlotScale_LinLin:
    RenderPrimitives(
        RectRenderer<GetterHeatmapCart<T>, TransformerLinLin>(getter, TransformerLinLin()),
        DrawList,
        gp.CurrentPlot->PlotRect);
    break;
  case ImPlotScale_LogLin:
    RenderPrimitives(
        RectRenderer<GetterHeatmapCart<T>, TransformerLogLin>(getter, TransformerLogLin()),
        DrawList,
        gp.CurrentPlot->PlotRect);
    break;
    ;
  case ImPlotScale_LinLog:
    RenderPrimitives(
        RectRenderer<GetterHeatmapCart<T>, TransformerLinLog>(getter, TransformerLinLog()),
        DrawList,
        gp.CurrentPlot->PlotRect);
    break;
    ;
  case ImPlotScale_LogLog:
    RenderPrimitives(
        RectRenderer<GetterHeatmapCart<T>, TransformerLogLog>(getter, TransformerLogLog()),
        DrawList,
        gp.CurrentPlot->PlotRect);
    break;
    ;
  }
  if (fmt != NULL) {
    const double w = (bounds_max.x - bounds_min.x) / cols;
    const double h = (bounds_max.y - bounds_min.y) / rows;
    const ImPlotPoint half_size(w * 0.5, h * 0.5);
    int i = 0;
    for (int r = 0; r < rows; ++r) {
      for (int c = 0; c < cols; ++c) {
        ImPlotPoint p;
        p.x = bounds_min.x + 0.5 * w + c * w;
        p.y = yref + ydir * (0.5 * h + r * h);
        ImVec2 px = transformer(p);
        char buff[32];
        sprintf(buff, fmt, values[i]);
        ImVec2 size = ImGui::CalcTextSize(buff);
        float t
            = ImClamp(ImRemap01((float)values[i], (float)scale_min, (float)scale_max), 0.0f, 1.0f);
        ImVec4 color = SampleColormap(t);
        ImU32 col = CalcTextColor(color);
        DrawList.AddText(px - size * 0.5f, col, buff);
        i++;
      }
    }
  }
}

template <typename T>
void PlotHeatmapCart(const char* label_id,
                     const T* values,
                     int rows,
                     int cols,
                     T scale_min,
                     T scale_max,
                     bool use_log_scale,
                     const char* fmt,
                     const ImPlotPoint& bounds_min,
                     const ImPlotPoint& bounds_max) {
  if (BeginItem(label_id)) {
    if (FitThisFrame()) {
      FitPoint(bounds_min);
      FitPoint(bounds_max);
    }
    ImDrawList& DrawList = *GetPlotDrawList();
    RenderHeatmapCart(TransformerLinLin(),
                      DrawList,
                      values,
                      rows,
                      cols,
                      scale_min,
                      scale_max,
                      use_log_scale,
                      fmt,
                      bounds_min,
                      bounds_max,
                      true);
    EndItem();
  }
}
template IMPLOT_API void PlotHeatmapCart<float>(const char*,
                                                const float*,
                                                int,
                                                int,
                                                float,
                                                float,
                                                bool,
                                                const char*,
                                                const ImPlotPoint&,
                                                const ImPlotPoint&);
template IMPLOT_API void PlotHeatmapCart<double>(const char*,
                                                 const double*,
                                                 int,
                                                 int,
                                                 double,
                                                 double,
                                                 bool,
                                                 const char*,
                                                 const ImPlotPoint&,
                                                 const ImPlotPoint&);

} // namespace ImPlot