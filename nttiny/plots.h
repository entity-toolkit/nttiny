#ifndef NTTINY_PLOTS_H
#define NTTINY_PLOTS_H

#include "defs.h"
#include "api.h"

#include <implot.h>
#include <toml.hpp>

#include <vector>
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

  void writeToFile(const std::string& fname, bool rewrite = false) {
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
      export_file << "[Plot." << m_ID << "]\n";
      export_file << data;
      export_file << "\n";
      export_file.close();
    } else {
      throw std::runtime_error("ERROR: Cannot open file.");
    }
  }
};

template <class T, ushort D>
class Ax {
protected:
  SimulationAPI<T, D>* m_sim;
  const int m_ID;

public:
  Ax(int id) : m_ID(id) {}
  virtual ~Ax() = default;
  virtual auto draw(ImPlotRect&) -> bool { return false; }
  virtual auto getId() -> int { return -1; }
  virtual auto exportMetadata() -> PlotMetadata { return PlotMetadata(); }
  virtual void importMetadata(const PlotMetadata&){};
  void bindSimulation(SimulationAPI<T, D>* sim) { this->m_sim = sim; }
};

} // namespace nttiny

#endif
