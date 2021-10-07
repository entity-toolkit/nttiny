#ifndef NTTINY_PLOTS_H
#define NTTINY_PLOTS_H

#include "defs.h"
#include "api.h"

#include <implot.h>
#include <toml.hpp>

#include <string>
#include <fstream>
#include <stdexcept>

namespace nttiny {

struct PlotMetadata {
  int m_ID;
  std::string m_type;
  bool m_log;
  float m_vmin, m_vmax;
  std::string m_cmap;
  int m_field_selected;

  void writeToFile(const std::string& fname, bool rewrite=false) {
    const toml::value data{{"ID", m_ID},
                           {"type", m_type},
                           {"log", m_log},
                           {"vmin", m_vmin},
                           {"vmax", m_vmax},
                           {"cmap", m_cmap},
                           {"field_selected", m_field_selected}};
    std::ofstream export_file;
    if (rewrite) {
      export_file.open(fname);
    } else {
      export_file.open(fname, std::fstream::app);
    }
    if (export_file.is_open()) {
      export_file << "[Plot " << m_ID << "]\n";
      export_file << data;
      export_file << "\n";
      export_file.close();
    } else {
      throw std::runtime_error("ERROR: Cannot open file.");
    }
  }
};

template <class T> class Ax {
protected:
  SimulationAPI<T> *m_sim;
  const int m_ID;

public:
  Ax(int id);
  virtual ~Ax() = 0;
  virtual auto draw() -> bool;
  virtual auto getId() -> int;
  virtual auto exportMetadata() -> PlotMetadata;
  void bindSimulation(SimulationAPI<T> *sim);
};

template <class T> class Plot2d : public Ax<T> {
protected:
  float m_scale{1.0f};

public:
  Plot2d(int id) : Ax<T>(id) {}
  ~Plot2d() override = default;
  void scale();
  auto close() -> bool;
};

template <class T> class Pcolor2d : public Plot2d<T> {
protected:
  float m_sidebar_w{60}, m_cmap_h{225};
  bool m_log{false};
  float m_plot_size{350};
  float m_vmin, m_vmax;
  ImPlotColormap m_cmap{ImPlotColormap_Jet};
  int m_field_selected{0};

public:
  Pcolor2d(int id, float vmin, float vmax)
      : Plot2d<T>(id), m_vmin(vmin), m_vmax(vmax) {}
  ~Pcolor2d() override = default;
  auto getId() -> int override { return this->m_ID; }
  auto draw() -> bool override;
  auto exportMetadata() -> PlotMetadata override;
};

  // TODO: 1d plot, linear, log linear and log log, multiple data

  // template<class T>
  // class Plot : virtual public Plot2d {
  // private:
  //   T *m_xvalues, *m_yvalues;
  //   int *n;
  // protected:
  //   float m_ratio;
  //   float m_plot_size{350};
  //   std::string m_xlabel, m_ylabel;
  // public:
  //   Plot(ImVec2 xlims, ImVec2 ylims, float ratio) : Plot2d(xlims, ylims),
  //   m_ratio(ratio) {} ~Plot() = default; void draw() override;
  // };
}

#endif
