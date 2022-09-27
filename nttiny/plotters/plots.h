#ifndef NTTINY_PLOTS_H
#define NTTINY_PLOTS_H

#include "defs.h"
#include "api.h"
#include "metadata.h"

#include <implot/implot.h>
#include <toml11/toml.hpp>

#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

namespace nttiny {

/**
 * @brief Parent class for all plots.
 */
template <class T, ushort D>
class Ax {
protected:
  SimulationAPI<T, D>* m_sim;
  const int m_ID;

public:
  Ax(int id) : m_ID(id) {}
  virtual ~Ax() = default;
  auto close() const -> bool { return ImGui::Button("delete"); }
  virtual auto draw(ImPlotRect&, UISettings&) -> bool = 0;
  virtual auto exportMetadata() -> PlotMetadata* = 0;
  virtual void importMetadata(const toml::value&) = 0;
  void bindSimulation(SimulationAPI<T, D>* sim) { this->m_sim = sim; }
  auto getId() const -> int { return this->m_ID; }
};

/**
 * @brief Parent class for all 2D plots.
 */
template <class T>
struct Plot2d : public Ax<T, 2> {
  Plot2d(int id) : Ax<T, 2>(id) {}
  ~Plot2d() override = default;
  void outlineDomain(UISettings&);

  bool m_share_axes{true};
};

/**
 * @brief 2D pseudocolor plot.
 */
template <class T>
class Pcolor2d : public Plot2d<T> {
protected:
  bool m_log{false};
  bool m_autoscale{false};
  T m_vmin{(T)0.0}, m_vmax{(T)0.0};
  ImPlotColormap m_cmap{ImPlotColormap_Jet};
  int m_field_selected{0};
  void rescaleMinMax();

public:
  Pcolor2d(int id) : Plot2d<T>(id) {}
  ~Pcolor2d() override = default;
  auto draw(ImPlotRect&, UISettings&) -> bool override;
  auto exportMetadata() -> PlotMetadata* override;
  void importMetadata(const toml::value&) override;
};

/**
 * @brief 2D scatter plot.
 */
template <class T>
class Scatter2d : public Plot2d<T> {
protected:
  bool* m_prtl_enabled{nullptr};
  const char** m_prtl_names;

public:
  Scatter2d(int id) : Plot2d<T>(id) {}
  ~Scatter2d() override = default;
  auto draw(ImPlotRect&, UISettings&) -> bool override;
  auto exportMetadata() -> PlotMetadata* override;
  void importMetadata(const toml::value&) override;
};

/**
 * @brief Dynamically updated 1D plot.
 */
template <class T, ushort D>
struct TimePlot : public Ax<T, D> {
protected:
  int m_buff_selected{0};
  bool m_autoscale_y{true};
  bool m_roll_x{true};
  bool m_log_y{false};
  T m_timespan{1000.0f};

public:
  TimePlot(int id) : Ax<T, D>(id) {}
  ~TimePlot() override = default;
  auto draw(ImPlotRect&, UISettings&) -> bool override;
  auto exportMetadata() -> PlotMetadata* override;
  void importMetadata(const toml::value&) override;
};

} // namespace nttiny

#endif
