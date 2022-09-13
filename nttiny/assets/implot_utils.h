#ifndef IMPORT_UTILS_H
#define IMPORT_UTILS_H

#include <implot.h>
#include <implot_internal.h>

#include <math.h>

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

#if defined __SSE__ || defined __x86_64__ || defined _M_X64
#  ifndef IMGUI_ENABLE_SSE
#    include <immintrin.h>
#  endif
static IMPLOT_INLINE float ImInvSqrt(float x) { return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(x))); }
#else
static IMPLOT_INLINE float ImInvSqrt(float x) { return 1.0f / sqrtf(x); }
#endif

namespace ImPlot {

namespace {
  template <typename T>
  struct MaxIdx {
    static const unsigned int Value;
  };
  template <>
  const unsigned int MaxIdx<unsigned short>::Value = 65535;
  template <>
  const unsigned int MaxIdx<unsigned int>::Value = 4294967295;
} // namespace

//-----------------------------------------------------------------------------
// TRANSFORMERS
//-----------------------------------------------------------------------------

// Transforms convert points in plot space (i.e. ImPlotPoint) to pixel space (i.e. ImVec2)

struct TransformerLin {
  TransformerLin(double pixMin, double pltMin, double, double m, double)
      : PixMin(pixMin), PltMin(pltMin), M(m) {}
  template <typename T>
  IMPLOT_INLINE float operator()(T p) const {
    return (float)(PixMin + M * (p - PltMin));
  }
  double PixMin, PltMin, M;
};

struct TransformerLog {
  TransformerLog(double pixMin, double pltMin, double pltMax, double m, double den)
      : Den(den), PltMin(pltMin), PltMax(pltMax), PixMin(pixMin), M(m) {}
  template <typename T>
  IMPLOT_INLINE float operator()(T p) const {
    p = p <= 0.0 ? IMPLOT_LOG_ZERO : p;
    double t = ImLog10(p / PltMin) / Den;
    p = ImLerp(PltMin, PltMax, (float)t);
    return (float)(PixMin + M * (p - PltMin));
  }
  double Den, PltMin, PltMax, PixMin, M;
};

template <typename TransformerX, typename TransformerY>
struct TransformerXY {
  TransformerXY(const ImPlotAxis& x_axis, const ImPlotAxis& y_axis)
      : Tx(x_axis.PixelMin, x_axis.Range.Min, x_axis.Range.Max, x_axis.LinM, x_axis.LogD),
        Ty(y_axis.PixelMin, y_axis.Range.Min, y_axis.Range.Max, y_axis.LinM, y_axis.LogD) {}

  TransformerXY(const ImPlotPlot& plot)
      : TransformerXY(plot.Axes[plot.CurrentX], plot.Axes[plot.CurrentY]) {}

  TransformerXY() : TransformerXY(*GImPlot->CurrentPlot) {}

  template <typename P>
  IMPLOT_INLINE ImVec2 operator()(const P& plt) const {
    ImVec2 out;
    out.x = Tx(plt.x);
    out.y = Ty(plt.y);
    return out;
  }
  TransformerX Tx;
  TransformerY Ty;
};

typedef TransformerXY<TransformerLin, TransformerLin> TransformerLinLin;
typedef TransformerXY<TransformerLin, TransformerLog> TransformerLinLog;
typedef TransformerXY<TransformerLog, TransformerLin> TransformerLogLin;
typedef TransformerXY<TransformerLog, TransformerLog> TransformerLogLog;

/// Renders primitive shapes in bulk as efficiently as possible.
template <typename Renderer>
IMPLOT_INLINE void
RenderPrimitives(const Renderer& renderer, ImDrawList& DrawList, const ImRect& cull_rect) {
  unsigned int prims = renderer.Prims;
  unsigned int prims_culled = 0;
  unsigned int idx = 0;
  const ImVec2 uv = DrawList._Data->TexUvWhitePixel;
  while (prims) {
    // find how many can be reserved up to end of current draw command's limit
    unsigned int cnt = ImMin(
        prims, (MaxIdx<ImDrawIdx>::Value - DrawList._VtxCurrentIdx) / Renderer::VtxConsumed);
    // make sure at least this many elements can be rendered to avoid situations where at the end of
    // buffer this slow path is not taken all the time
    if (cnt >= ImMin(64u, prims)) {
      if (prims_culled >= cnt)
        prims_culled -= cnt; // reuse previous reservation
      else {
        DrawList.PrimReserve(
            (cnt - prims_culled) * Renderer::IdxConsumed,
            (cnt - prims_culled)
                * Renderer::VtxConsumed); // add more elements to previous reservation
        prims_culled = 0;
      }
    } else {
      if (prims_culled > 0) {
        DrawList.PrimUnreserve(prims_culled * Renderer::IdxConsumed,
                               prims_culled * Renderer::VtxConsumed);
        prims_culled = 0;
      }
      cnt = ImMin(prims,
                  (MaxIdx<ImDrawIdx>::Value - 0 /*DrawList._VtxCurrentIdx*/)
                      / Renderer::VtxConsumed);
      DrawList.PrimReserve(cnt * Renderer::IdxConsumed,
                           cnt * Renderer::VtxConsumed); // reserve new draw command
    }
    prims -= cnt;
    for (unsigned int ie = idx + cnt; idx != ie; ++idx) {
      if (!renderer(DrawList, cull_rect, uv, idx)) prims_culled++;
    }
  }
  if (prims_culled > 0)
    DrawList.PrimUnreserve(prims_culled * Renderer::IdxConsumed,
                           prims_culled * Renderer::VtxConsumed);
}

} // namespace ImPlot

#endif