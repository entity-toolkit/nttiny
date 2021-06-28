#include "defs.h"
#include "vis.h"
#include "api.h"

#include <plog/Log.h>
#include <plog/Init.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/ColorConsoleAppender.h>

#include <string>

class FakeSimulation : public SimulationAPI<float> {
public:
  float *m_data1, *m_data2;
  FakeSimulation(int sx, int sy) : SimulationAPI<float>{sx, sy} {
    this->m_data1 = new float[sx * sy];
    this->m_data2 = new float[sx * sy];
  }
  ~FakeSimulation() {
    delete[] this->m_data1;
    delete[] this->m_data2;
  }
  void setData() override {
    this->m_timestep = 0;
    auto f_sx{static_cast<float>(this->m_sx)};
    auto f_sy{static_cast<float>(this->m_sy)};
    for (int j{0}; j < this->m_sy; ++j) {
      auto f_j{static_cast<float>(j)};
      for (int i{0}; i < this->m_sx; ++i) {
        auto f_i{static_cast<float>(i)};
        this->m_data1[i * this->m_sy + j] =
            0.5f * (f_i / f_sx) + 0.5f * (f_j / f_sy);
        this->m_data2[i * this->m_sy + j] = (f_i / f_sx) * (f_j / f_sy);
      }
    }
    try {
      this->fields.insert({{"data1", this->m_data1}, {"data2", this->m_data2}});
    } catch (std::exception err) {
      PLOGE << err.what();
    }
  }
  void stepFwd() override {
    ++this->m_timestep;
    for (int j{0}; j < this->m_sy; ++j) {
      for (int i{0}; i < this->m_sx; ++i) {
        this->m_data1[i * this->m_sy + j] =
            this->m_data1[i * this->m_sy + j] + 0.001f;
        this->m_data2[i * this->m_sy + j] =
            this->m_data2[i * this->m_sy + j] + 0.001f;
      }
    }
  }
  void stepBwd() override {
    --this->m_timestep;
    for (int j{0}; j < this->m_sy; ++j) {
      for (int i{0}; i < this->m_sx; ++i) {
        this->m_data1[i * this->m_sy + j] =
            this->m_data1[i * this->m_sy + j] - 0.001f;
        this->m_data2[i * this->m_sy + j] =
            this->m_data2[i * this->m_sy + j] - 0.001f;
      }
    }
  }
};

auto main() -> int {
  static plog::ColorConsoleAppender<plog::TxtFormatter> console_appender;
  plog::Severity max_severity;
#ifdef VERBOSE
  max_severity = plog::verbose;
#elif DEBUG
  max_severity = plog::debug;
#else
  max_severity = plog::warning;
#endif
  plog::init(max_severity, &console_appender);

  FakeSimulation sim(100, 150);
  sim.setData();
  //
  // Visualization<float> vis;
  // vis.setTPSLimit(30.0f);
  // vis.bindSimulation(&sim);
  // vis.loop();
  return 0;
}
