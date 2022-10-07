#ifndef IMPLOT_TRANSFORMERS_H
#define IMPLOT_TRANSFORMERS_H

#include <implot/implot.h>
#include <implot/implot_internal.h>

#ifndef IMPLOT_NO_FORCE_INLINE
#  ifdef _MSC_VER
#    define IMPLOT_INLINE __forceinline
#  elif defined(__GNUC__)
#    define IMPLOT_INLINE inline __attribute__((__always_inline__))
#  elif defined(__CLANG__)
#    if __has_attribute(__always_inline__)
#      define IMPLOT_INLINE inline __attribute__((__always_inline__))
#    else
#      define IMPLOT_INLINE inline
#    endif
#  else
#    define IMPLOT_INLINE inline
#  endif
#else
#  define IMPLOT_INLINE inline
#endif

namespace ImPlot {

struct Transformer1 {
  Transformer1(double pixMin,
               double pltMin,
               double pltMax,
               double m,
               double scaMin,
               double scaMax,
               ImPlotTransform fwd,
               void* data)
      : ScaMin(scaMin),
        ScaMax(scaMax),
        PltMin(pltMin),
        PltMax(pltMax),
        PixMin(pixMin),
        M(m),
        TransformFwd(fwd),
        TransformData(data) {}

  template <typename T>
  IMPLOT_INLINE float operator()(T p) const {
    if (TransformFwd != NULL) {
      double s = TransformFwd(p, TransformData);
      double t = (s - ScaMin) / (ScaMax - ScaMin);
      p = PltMin + (PltMax - PltMin) * t;
    }
    return (float)(PixMin + M * (p - PltMin));
  }

  double ScaMin, ScaMax, PltMin, PltMax, PixMin, M;
  ImPlotTransform TransformFwd;
  void* TransformData;
};

struct Transformer2 {
  Transformer2(const ImPlotAxis& x_axis, const ImPlotAxis& y_axis)
      : Tx(x_axis.PixelMin,
           x_axis.Range.Min,
           x_axis.Range.Max,
           x_axis.ScaleToPixel,
           x_axis.ScaleMin,
           x_axis.ScaleMax,
           x_axis.TransformForward,
           x_axis.TransformData),
        Ty(y_axis.PixelMin,
           y_axis.Range.Min,
           y_axis.Range.Max,
           y_axis.ScaleToPixel,
           y_axis.ScaleMin,
           y_axis.ScaleMax,
           y_axis.TransformForward,
           y_axis.TransformData) {}

  Transformer2(const ImPlotPlot& plot)
      : Transformer2(plot.Axes[plot.CurrentX], plot.Axes[plot.CurrentY]) {}

  Transformer2() : Transformer2(*GImPlot->CurrentPlot) {}

  template <typename P>
  IMPLOT_INLINE ImVec2 operator()(const P& plt) const {
    ImVec2 out;
    out.x = Tx(plt.x);
    out.y = Ty(plt.y);
    return out;
  }

  template <typename T>
  IMPLOT_INLINE ImVec2 operator()(T x, T y) const {
    ImVec2 out;
    out.x = Tx(x);
    out.y = Ty(y);
    return out;
  }

  Transformer1 Tx;
  Transformer1 Ty;
};

} // namespace ImPlot

#endif // IMPLOT_TRANSFORMERS_H