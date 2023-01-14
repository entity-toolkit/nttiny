#ifndef IMPLOT_RENDERERS_H
#define IMPLOT_RENDERERS_H

#include "implot_transformers.h"

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

  namespace {
    template <typename T>
    struct MaxIdx {
      static const unsigned int Value;
    };
    template <>
    const unsigned int MaxIdx<unsigned short>::Value = 65535;
    template <>
    const unsigned int MaxIdx<unsigned int>::Value = 4294967295;
  }    // namespace

  struct RectC {
    ImPlotPoint Pos;
    ImPlotPoint HalfSize;
    ImU32       Color;
  };

  struct ArcC {
    ImPlotPoint Min, Max;
    ImU32       Color;
  };

  IMPLOT_INLINE void PrimRectFill(ImDrawList&   draw_list,
                                  const ImVec2& Pmin,
                                  const ImVec2& Pmax,
                                  ImU32         col,
                                  const ImVec2& uv) {
    draw_list._VtxWritePtr[0].pos   = Pmin;
    draw_list._VtxWritePtr[0].uv    = uv;
    draw_list._VtxWritePtr[0].col   = col;
    draw_list._VtxWritePtr[1].pos   = Pmax;
    draw_list._VtxWritePtr[1].uv    = uv;
    draw_list._VtxWritePtr[1].col   = col;
    draw_list._VtxWritePtr[2].pos.x = Pmin.x;
    draw_list._VtxWritePtr[2].pos.y = Pmax.y;
    draw_list._VtxWritePtr[2].uv    = uv;
    draw_list._VtxWritePtr[2].col   = col;
    draw_list._VtxWritePtr[3].pos.x = Pmax.x;
    draw_list._VtxWritePtr[3].pos.y = Pmin.y;
    draw_list._VtxWritePtr[3].uv    = uv;
    draw_list._VtxWritePtr[3].col   = col;
    draw_list._VtxWritePtr += 4;
    draw_list._IdxWritePtr[0] = (ImDrawIdx)(draw_list._VtxCurrentIdx);
    draw_list._IdxWritePtr[1] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 1);
    draw_list._IdxWritePtr[2] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 2);
    draw_list._IdxWritePtr[3] = (ImDrawIdx)(draw_list._VtxCurrentIdx);
    draw_list._IdxWritePtr[4] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 1);
    draw_list._IdxWritePtr[5] = (ImDrawIdx)(draw_list._VtxCurrentIdx + 3);
    draw_list._IdxWritePtr += 6;
    draw_list._VtxCurrentIdx += 4;
  }

  IMPLOT_INLINE void PrimArcFill(ImDrawList&   draw_list,
                                 const ImVec2  P_arcup[],
                                 const ImVec2  P_arcdown[],
                                 const int&    ntheta,
                                 ImU32         col,
                                 const ImVec2& uv) {
    // adding `2 * (ntheta + 1)` points
    draw_list._VtxWritePtr[0].pos = P_arcdown[0];
    for (int i { 1 }; i <= (ntheta + 1); ++i) {
      draw_list._VtxWritePtr[i].pos = P_arcup[i - 1];
    }
    for (int i { ntheta + 2 }; i < 2 * (ntheta + 1); ++i) {
      draw_list._VtxWritePtr[i].pos = P_arcdown[2 * (ntheta + 1) - i];
    }
    for (int i { 0 }; i < 2 * (ntheta + 1); ++i) {
      draw_list._VtxWritePtr[i].uv  = uv;
      draw_list._VtxWritePtr[i].col = col;
    }
    draw_list._VtxWritePtr += 2 * (ntheta + 1);

    int n { 0 };
    for (int i { 0 }; i < ntheta; ++i) {
      draw_list._IdxWritePtr[n + 0] = (ImDrawIdx)(draw_list._VtxCurrentIdx) + (i + 1);
      draw_list._IdxWritePtr[n + 1]
        = (ImDrawIdx)(draw_list._VtxCurrentIdx) + 2 * ntheta + 1 - i;
      if (i == 0) {
        draw_list._IdxWritePtr[n + 2] = (ImDrawIdx)(draw_list._VtxCurrentIdx);
      } else {
        draw_list._IdxWritePtr[n + 2]
          = (ImDrawIdx)(draw_list._VtxCurrentIdx) + 2 * ntheta + 2 - i;
      }
      draw_list._IdxWritePtr[n + 3] = (ImDrawIdx)(draw_list._VtxCurrentIdx) + (i + 1);
      draw_list._IdxWritePtr[n + 4] = (ImDrawIdx)(draw_list._VtxCurrentIdx) + (i + 2);
      draw_list._IdxWritePtr[n + 5]
        = (ImDrawIdx)(draw_list._VtxCurrentIdx) + 2 * ntheta + 1 - i;
      n += 6;
    }
    draw_list._IdxWritePtr += 6 * ntheta;
    draw_list._VtxCurrentIdx += 2 * (ntheta + 1);
  }

  struct FitterRect {
    FitterRect(const ImPlotPoint& pmin, const ImPlotPoint& pmax) : Pmin(pmin), Pmax(pmax) {}
    FitterRect(const ImPlotRect& rect) : FitterRect(rect.Min(), rect.Max()) {}
    void Fit(ImPlotAxis& x_axis, ImPlotAxis& y_axis) const {
      x_axis.ExtendFitWith(y_axis, Pmin.x, Pmin.y);
      y_axis.ExtendFitWith(x_axis, Pmin.y, Pmin.x);
      x_axis.ExtendFitWith(y_axis, Pmax.x, Pmax.y);
      y_axis.ExtendFitWith(x_axis, Pmax.y, Pmax.x);
    }
    const ImPlotPoint Pmin;
    const ImPlotPoint Pmax;
  };

  struct RendererBase {
    RendererBase(int prims, int idx_consumed, int vtx_consumed)
      : Prims(prims), IdxConsumed(idx_consumed), VtxConsumed(vtx_consumed) {}
    const int    Prims;
    Transformer2 Transformer;
    const int    IdxConsumed;
    const int    VtxConsumed;
  };

  template <typename _Getter>
  struct RendererRectC : RendererBase {
    RendererRectC(const _Getter& getter) : RendererBase(getter.Count, 6, 4), Getter(getter) {}
    void Init(ImDrawList& draw_list) const {
      UV = draw_list._Data->TexUvWhitePixel;
    }
    IMPLOT_INLINE bool Render(ImDrawList& draw_list, const ImRect& cull_rect, int prim) const {
      RectC  rect = Getter(prim);
      ImVec2 P1
        = this->Transformer(rect.Pos.x - rect.HalfSize.x, rect.Pos.y - rect.HalfSize.y);
      ImVec2 P2
        = this->Transformer(rect.Pos.x + rect.HalfSize.x, rect.Pos.y + rect.HalfSize.y);
      if ((rect.Color & IM_COL32_A_MASK) == 0
          || !cull_rect.Overlaps(ImRect(ImMin(P1, P2), ImMax(P1, P2))))
        return false;
      PrimRectFill(draw_list, P1, P2, rect.Color, UV);
      return true;
    }
    const _Getter& Getter;
    mutable ImVec2 UV;
  };

  template <typename _Getter>
  struct RendererArcC : RendererBase {
    static const int NTheta { 5 };

    RendererArcC(const _Getter& getter)
      : RendererBase(getter.Count, 6 * NTheta, 2 * (NTheta + 1)), Getter(getter) {}
    void Init(ImDrawList& draw_list) const {
      UV = draw_list._Data->TexUvWhitePixel;
    }
    IMPLOT_INLINE bool Render(ImDrawList& draw_list, const ImRect&, int prim) const {
      ArcC   arc = Getter(prim);
      ImVec2 P_arcup[NTheta + 1], P_arcdown[NTheta + 1];
      double r_1, theta_1, r_2, theta_2;
      r_1     = arc.Min.x;
      r_2     = arc.Max.x;
      theta_1 = arc.Min.y;
      theta_2 = arc.Max.y;
      for (int i { 0 }; i < NTheta + 1; ++i) {
        double th { theta_1 + (theta_2 - theta_1) * (double)(i) / (double)(NTheta) };
        P_arcup[i].x   = r_2 * sin(th);
        P_arcup[i].y   = r_2 * cos(th);
        P_arcdown[i].x = r_1 * sin(th);
        P_arcdown[i].y = r_1 * cos(th);
        P_arcup[i]     = this->Transformer(P_arcup[i]);
        P_arcdown[i]   = this->Transformer(P_arcdown[i]);
      }

      if ((arc.Color & IM_COL32_A_MASK) == 0)
        return false;
      PrimArcFill(draw_list, P_arcup, P_arcdown, NTheta, arc.Color, UV);
      return true;
    }
    const _Getter& Getter;
    mutable ImVec2 UV;
  };

  /// Renders primitive shapes in bulk as efficiently as possible.
  template <class _Renderer>
  void RenderPrimitivesEx(const _Renderer& renderer,
                          ImDrawList&      draw_list,
                          const ImRect&    cull_rect) {
    unsigned int prims        = renderer.Prims;
    unsigned int prims_culled = 0;
    unsigned int idx          = 0;
    renderer.Init(draw_list);
    while (prims) {
      // find how many can be reserved up to end of current draw command's limit
      unsigned int cnt = ImMin(
        prims, (MaxIdx<ImDrawIdx>::Value - draw_list._VtxCurrentIdx) / renderer.VtxConsumed);
      // make sure at least this many elements can be rendered to avoid situations where at the
      // end of buffer this slow path is not taken all the time
      if (cnt >= ImMin(64u, prims)) {
        if (prims_culled >= cnt)
          prims_culled -= cnt;    // reuse previous reservation
        else {
          // add more elements to previous reservation
          draw_list.PrimReserve((cnt - prims_culled) * renderer.IdxConsumed,
                                (cnt - prims_culled) * renderer.VtxConsumed);
          prims_culled = 0;
        }
      } else {
        if (prims_culled > 0) {
          draw_list.PrimUnreserve(prims_culled * renderer.IdxConsumed,
                                  prims_culled * renderer.VtxConsumed);
          prims_culled = 0;
        }
        cnt = ImMin(
          prims,
          (MaxIdx<ImDrawIdx>::Value - 0 /*draw_list._VtxCurrentIdx*/) / renderer.VtxConsumed);
        // reserve new draw command
        draw_list.PrimReserve(cnt * renderer.IdxConsumed, cnt * renderer.VtxConsumed);
      }
      prims -= cnt;
      for (unsigned int ie = idx + cnt; idx != ie; ++idx) {
        if (!renderer.Render(draw_list, cull_rect, idx))
          prims_culled++;
      }
    }
    if (prims_culled > 0)
      draw_list.PrimUnreserve(prims_culled * renderer.IdxConsumed,
                              prims_culled * renderer.VtxConsumed);
  }

  template <template <class> class _Renderer, class _Getter, typename... Args>
  void RenderPrimitives1(const _Getter& getter, Args... args) {
    ImDrawList&   draw_list = *GetPlotDrawList();
    const ImRect& cull_rect = GetCurrentPlot()->PlotRect;
    RenderPrimitivesEx(_Renderer<_Getter>(getter, args...), draw_list, cull_rect);
  }

}    // namespace ImPlot

#endif    // IMPLOT_RENDERERS_H