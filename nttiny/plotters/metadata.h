#ifndef METADATA_H
#define METADATA_H

#include "defs.h"
#include <toml11/toml.hpp>

struct PlotMetadata {
  const int m_ID;
  toml::table metadata;

  PlotMetadata(int id) : m_ID(id) { metadata.insert({"ID", id}); }
  ~PlotMetadata() = default;
  virtual void setMetadata() = 0;
  void writeToFile(const std::string& fname, bool rewrite = false) {
    setMetadata();
    std::ofstream export_file;
    if (rewrite) {
      export_file.open(fname);
    } else {
      export_file.open(fname, std::fstream::app);
    }
    if (export_file.is_open()) {
      export_file << "[Plot." << m_ID << "]\n";
      for (auto& [key, value] : metadata) {
        export_file << key << " = " << value << "\n";
      }
      export_file << "\n";
      export_file.close();
    } else {
      throw std::runtime_error("ERROR: Cannot open file.");
    }
  }
};

struct Pcolor2dMetadata : public PlotMetadata {
  const std::string m_type{"Pcolor2d"};
  bool m_log, m_autoscale;
  float m_vmin, m_vmax;
  std::string m_cmap;
  int m_field_selected;
  Pcolor2dMetadata(int id) : PlotMetadata(id) {}
  void setMetadata() override {
    (this->metadata).insert({"type", m_type});
    (this->metadata).insert({"log", m_log});
    (this->metadata).insert({"autoscale", m_autoscale});
    (this->metadata).insert({"vmin", m_vmin});
    (this->metadata).insert({"vmax", m_vmax});
    (this->metadata).insert({"cmap", m_cmap});
    (this->metadata).insert({"field_selected", m_field_selected});
  }
};

struct Scatter2dMetadata : public PlotMetadata {
  const std::string m_type{"Scatter2d"};
  Scatter2dMetadata(int id) : PlotMetadata(id) {}
  void setMetadata() override { (this->metadata).insert({"type", m_type}); }
};

template <class T>
struct TimeplotMetadata : public PlotMetadata {
  const std::string m_type{"TimePlot"};
  int m_buff_selected;
  bool m_autoscale_y;
  bool m_roll_x;
  bool m_log_y;
  T m_timespan;

  TimeplotMetadata(int id) : PlotMetadata(id) {}

  void setMetadata() override {
    (this->metadata).insert({"type", m_type});
    (this->metadata).insert({"buff_selected", m_buff_selected});
    (this->metadata).insert({"autoscale_y", m_autoscale_y});
    (this->metadata).insert({"roll_x", m_roll_x});
    (this->metadata).insert({"log_y", m_log_y});
    (this->metadata).insert({"timespan", m_timespan});
  }
};

#endif