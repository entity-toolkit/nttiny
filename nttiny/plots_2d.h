#ifndef NTTINY_PLOTS_2D_H
#define NTTINY_PLOTS_2D_H

#include "defs.h"
#include "api.h"
#include "plots.h"

#include <implot.h>
#include <toml.hpp>

#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

namespace nttiny {

template <class T>
struct Plot2d : public Ax<T, 2> {
  Plot2d(int id) : Ax<T, 2>(id) {}
  ~Plot2d() override = default;
  auto close(const int& w = -1) -> bool;
  auto getId() -> int override { return this->m_ID; }
  void outlineDomain();

  bool m_share_axes{true};
};

template <class T>
class Pcolor2d : public Plot2d<T> {
protected:
  bool m_log{false};
  bool m_autoscale{true};
  T m_vmin, m_vmax;
  ImPlotColormap m_cmap{ImPlotColormap_Jet};
  int m_field_selected{0};
  void rescaleMinMax();

public:
  Pcolor2d(int id, T vmin, T vmax) : Plot2d<T>(id), m_vmin(vmin), m_vmax(vmax) {}
  ~Pcolor2d() override = default;
  auto draw(ImPlotRect&) -> bool override;
  auto exportMetadata() -> PlotMetadata override;
  void importMetadata(const PlotMetadata&) override;
};

template <class T>
class Scatter2d : public Plot2d<T> {
protected:
  bool* m_prtl_enabled{nullptr};
  const char** m_prtl_names;

public:
  Scatter2d(int id) : Plot2d<T>(id) {}
  ~Scatter2d() override = default;
  auto draw(ImPlotRect&) -> bool override;
  auto exportMetadata() -> PlotMetadata override;
  void importMetadata(const PlotMetadata&) override;
};

// TODO: 1d plot, linear, log linear and log log, multiple data

} // namespace nttiny

#endif
