#ifndef GLENGINE_MPL_H
#define GLENGINE_MPL_H

#include "defs.h"

#include <implot.h>

#include <string>

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

class Plot : public Plot2d {
private:
  const float m_ratio;
protected:
  float m_plot_size{350};
  std::string m_xlabel, m_ylabel;
public:
  Plot(ImVec2 xlims, ImVec2 ylims, float ratio) : Plot2d(xlims, ylims), m_ratio(ratio) {}
  ~Plot() = default;
  template<typename T>
  void draw(T *x_values, T *y_values, int n, const std::string &label=std::string());
};

class Pcolor2d : public Plot2d {
private:
  const float m_sidebar_w{60}, m_cmap_h{225};
protected:
  bool m_log;
  float m_plot_size{350};
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
