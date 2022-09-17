#include "implot_utils.h"
#include "implot_heatmap_polar.h"

#include <implot.h>
#include <implot_internal.h>

namespace ImPlot {

template <typename TGetter, typename TTransformer>
struct ArcRenderer {
  IMPLOT_INLINE ArcRenderer(const TGetter& getter, const TTransformer& transformer)
      : Getter(getter), Transformer(transformer), Prims(Getter.Count) {}
  IMPLOT_INLINE bool
  operator()(ImDrawList& DrawList, const ImRect& cull_rect, const ImVec2& uv, int prim) const {
    ArcInfo arc = Getter(prim);
    ImVec2 P_arcup[ntheta + 1], P_arcdown[ntheta + 1];
    double r_1, theta_1, r_2, theta_2;
    r_1 = arc.Min.x;
    r_2 = arc.Max.x;
    theta_1 = arc.Min.y;
    theta_2 = arc.Max.y;

    for (int i{0}; i < ntheta + 1; ++i) {
      double th{theta_1 + (theta_2 - theta_1) * (double)(i) / (double)(ntheta)};
      P_arcup[i].x = r_2 * sin(th);
      P_arcup[i].y = r_2 * cos(th);

      P_arcdown[i].x = r_1 * sin(th);
      P_arcdown[i].y = r_1 * cos(th);

      P_arcup[i] = Transformer(P_arcup[i]);
      P_arcdown[i] = Transformer(P_arcdown[i]);
    }

    (void)(cull_rect);
    if ((arc.Color & IM_COL32_A_MASK) == 0) return false;

    // adding `2 * (ntheta + 1)` points
    DrawList._VtxWritePtr[0].pos = P_arcdown[0];
    for (int i{1}; i <= (ntheta + 1); ++i) {
      DrawList._VtxWritePtr[i].pos = P_arcup[i - 1];
    }
    for (int i{ntheta + 2}; i < 2 * (ntheta + 1); ++i) {
      DrawList._VtxWritePtr[i].pos = P_arcdown[2 * (ntheta + 1) - i];
    }
    for (int i{0}; i < 2 * (ntheta + 1); ++i) {
      DrawList._VtxWritePtr[i].uv = uv;
      DrawList._VtxWritePtr[i].col = arc.Color;
    }
    DrawList._VtxWritePtr += 2 * (ntheta + 1);

    int n{0};
    for (int i{0}; i < ntheta; ++i) {
      DrawList._IdxWritePtr[n + 0] = (ImDrawIdx)(DrawList._VtxCurrentIdx) + (i + 1);
      DrawList._IdxWritePtr[n + 1] = (ImDrawIdx)(DrawList._VtxCurrentIdx) + 2 * ntheta + 1 - i;
      if (i == 0) {
        DrawList._IdxWritePtr[n + 2] = (ImDrawIdx)(DrawList._VtxCurrentIdx);
      } else {
        DrawList._IdxWritePtr[n + 2] = (ImDrawIdx)(DrawList._VtxCurrentIdx) + 2 * ntheta + 2 - i;
      }
      DrawList._IdxWritePtr[n + 3] = (ImDrawIdx)(DrawList._VtxCurrentIdx) + (i + 1);
      DrawList._IdxWritePtr[n + 4] = (ImDrawIdx)(DrawList._VtxCurrentIdx) + (i + 2);
      DrawList._IdxWritePtr[n + 5] = (ImDrawIdx)(DrawList._VtxCurrentIdx) + 2 * ntheta + 1 - i;
      n += 6;
    }
    DrawList._IdxWritePtr += 6 * ntheta;
    DrawList._VtxCurrentIdx += 2 * (ntheta + 1);
    return true;
  }
  const TGetter& Getter;
  const TTransformer& Transformer;
  const int Prims;
  static const int ntheta{5};
  static const int IdxConsumed = 6 * ntheta;
  static const int VtxConsumed = 2 * (ntheta + 1);
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
struct GetterHeatmapPolar {
  GetterHeatmapPolar(const T* values,
                     int rows,
                     int cols,
                     T scale_min,
                     T scale_max,
                     const T* r_array,
                     const T* theta_array,
                     const bool use_log_scale)
      : Values(values),
        Rows(rows),
        Cols(cols),
        Count(rows * cols),
        ScaleMin(scale_min),
        ScaleMax(scale_max),
        RArray(r_array),
        ThetaArray(theta_array),
        UseLogScale(use_log_scale) {}

  template <typename I>
  IMPLOT_INLINE ArcInfo operator()(I idx) const {
    T val;
    if (UseLogScale) {
      val = (T)(QLOGSCALE(Values[idx]));
    } else {
      val = (T)Values[idx];
    }
    const int r = idx / Cols;
    const int c = idx % Cols;

    T rlow{RArray[c]}, rhigh{RArray[c + 1]};
    T th1{ThetaArray[r]}, th2{ThetaArray[r + 1]};

    ArcInfo arc;
    arc.Min.x = rlow;
    arc.Min.y = th1;
    arc.Max.x = rhigh;
    arc.Max.y = th2;
    const float t
        = ImClamp((float)ImRemap01((float)val, (float)ScaleMin, (float)ScaleMax), 0.0f, 1.0f);
    arc.Color = GImPlot->ColormapData.LerpTable(GImPlot->Style.Colormap, t);

    return arc;
  }
  const T* const Values;
  const int Rows, Cols, Count;
  const T ScaleMin, ScaleMax;
  const T *RArray, *ThetaArray;
  const bool UseLogScale;
};

#undef SIGN
#undef ABS
#undef QLOGSCALE

template <typename T, typename Transformer>
void RenderHeatmapPolar(Transformer transformer,
                        ImDrawList& DrawList,
                        const T* values,
                        int rows,
                        int cols,
                        T scale_min,
                        T scale_max,
                        T* r_array,
                        T* theta_array,
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
  const T yref = reverse_y ? bounds_max.y : bounds_min.y;
  const T ydir = reverse_y ? -1 : 1;
  GetterHeatmapPolar<T> getter(
      values, rows, cols, scale_min, scale_max, r_array, theta_array, use_log_scale);
  switch (GetCurrentScale()) {
  case ImPlotScale_LinLin:
    RenderPrimitives(
        ArcRenderer<GetterHeatmapPolar<T>, TransformerLinLin>(getter, TransformerLinLin()),
        DrawList,
        gp.CurrentPlot->PlotRect);
    break;
  case ImPlotScale_LogLin:
    RenderPrimitives(
        ArcRenderer<GetterHeatmapPolar<T>, TransformerLogLin>(getter, TransformerLogLin()),
        DrawList,
        gp.CurrentPlot->PlotRect);
    break;
    ;
  case ImPlotScale_LinLog:
    RenderPrimitives(
        ArcRenderer<GetterHeatmapPolar<T>, TransformerLinLog>(getter, TransformerLinLog()),
        DrawList,
        gp.CurrentPlot->PlotRect);
    break;
    ;
  case ImPlotScale_LogLog:
    RenderPrimitives(
        ArcRenderer<GetterHeatmapPolar<T>, TransformerLogLog>(getter, TransformerLogLog()),
        DrawList,
        gp.CurrentPlot->PlotRect);
    break;
    ;
  }
  if (fmt != NULL) {
    const T w = (bounds_max.x - bounds_min.x) / cols;
    const T h = (bounds_max.y - bounds_min.y) / rows;
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
void PlotHeatmapPolar(const char* label_id,
                      const T* values,
                      int rows,
                      int cols,
                      T scale_min,
                      T scale_max,
                      T* r_array,
                      T* theta_array,
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
    RenderHeatmapPolar(TransformerLinLin(),
                       DrawList,
                       values,
                       rows,
                       cols,
                       scale_min,
                       scale_max,
                       r_array,
                       theta_array,
                       use_log_scale,
                       fmt,
                       bounds_min,
                       bounds_max,
                       true);
    EndItem();
  }
}

template IMPLOT_API void PlotHeatmapPolar<float>(const char*,
                                                 const float*,
                                                 int,
                                                 int,
                                                 float,
                                                 float,
                                                 float*,
                                                 float*,
                                                 bool,
                                                 const char*,
                                                 const ImPlotPoint&,
                                                 const ImPlotPoint&);
template IMPLOT_API void PlotHeatmapPolar<double>(const char*,
                                                  const double*,
                                                  int,
                                                  int,
                                                  double,
                                                  double,
                                                  double*,
                                                  double*,
                                                  bool,
                                                  const char*,
                                                  const ImPlotPoint&,
                                                  const ImPlotPoint&);

} // namespace ImPlot