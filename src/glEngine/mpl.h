#ifndef GLENGINE_MPL_H
#define GLENGINE_MPL_H

#include "defs.h"

#include <implot.h>

#include <algorithm>

static int CNTR = 0;

class Plot2d {
protected:
  const int m_ID;
  ImVec2 m_xlims, m_ylims;
  float m_scale{1.0f};
public:
  Plot2d(ImVec2 xlims, ImVec2 ylims);
  ~Plot2d() = default;
  void scale();
};

// TODO: add field selector

class Pcolor2d : public Plot2d {
private:
  const int m_sidebar_w{60}, m_cmap_h{225};
protected:
  bool m_log;
  int m_plot_size{350};
  float m_vmin, m_vmax;
  ImPlotColormap m_cmap {ImPlotColormap_Jet};
public:
  Pcolor2d(float vmin, float vmax, ImVec2 xlims, ImVec2 ylims) :
    Plot2d(xlims, ylims), m_vmin(vmin), m_vmax(vmax) {}
  ~Pcolor2d() = default;
  template<typename T>
  void draw(T *values, int sx, int sy);
};

// TODO: 1d plot, linear, log linear and log log, multiple data

// TODO: add a way to add plots

#endif
