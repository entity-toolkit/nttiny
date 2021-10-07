#include "nttiny/vis.h"
#include "nttiny/api.h"

#include <iostream>
#include <string>
#include <stdexcept>

class FakeSimulation : public nttiny::SimulationAPI<float> {
public:
  nttiny::Data<float> ex;
  nttiny::Data<float> bx;

  nttiny::Data<float> electrons_x;
  nttiny::Data<float> electrons_y;
  nttiny::Data<float> positrons_x;
  nttiny::Data<float> positrons_y;

  FakeSimulation(int sx, int sy) : nttiny::SimulationAPI<float>{sx, sy} {
    this->ex.allocate(sx * sy);
    this->bx.allocate(sx * sy);
    this->ex.set_size(0, sx);
    this->ex.set_size(1, sy);
    this->bx.set_size(0, sx);
    this->bx.set_size(1, sy);
    this->ex.set_dimension(2);
    this->bx.set_dimension(2);

    this->electrons_x.allocate(1000);
    this->electrons_y.allocate(1000);
    this->positrons_x.allocate(1000);
    this->positrons_y.allocate(1000);

    this->electrons_x.set_size(0, 1000);
    this->electrons_y.set_size(0, 1000);
    this->positrons_x.set_size(0, 1000);
    this->positrons_y.set_size(0, 1000);
  }
  ~FakeSimulation() = default;
  void setData() override {
    m_x1x2_extent[0] = 0.0f;
    m_x1x2_extent[1] = 1.0f;
    m_x1x2_extent[2] = 0.0f;
    m_x1x2_extent[3] = 1.5f;
    this->m_timestep = 0;
    auto f_sx{static_cast<float>(this->m_sx)};
    auto f_sy{static_cast<float>(this->m_sy)};
    for (int j{0}; j < this->m_sy; ++j) {
      auto f_j{static_cast<float>(j)};
      for (int i{0}; i < this->m_sx; ++i) {
        auto f_i{static_cast<float>(i)};
        this->ex.set(i * this->m_sy + j, 0.5f * (f_i / f_sx) + 0.5f * (f_j / f_sy));
        this->bx.set(i * this->m_sy + j, (f_i / f_sx) * (f_j / f_sy));
      }
    }
    this->fields.insert({{"ex", &(this->ex)}, {"bx", &(this->bx)}});

    for (int i = 0; i < 1000; ++i) {
      this->electrons_x.set(i, m_x1x2_extent[1] * i / 1000.0);
      this->electrons_y.set(i, m_x1x2_extent[3] * i / 1000.0);
      this->positrons_x.set(i, 0.1f + m_x1x2_extent[1] * i / 1000.0);
      this->positrons_y.set(i, 0.1f + m_x1x2_extent[3] * i / 1000.0);
    }
    this->particles.insert({{"electrons",
                                {&(this->electrons_x),
                                 &(this->electrons_y)}
                             },{
                             "positrons",
                                {&(this->positrons_x),
                                 &(this->positrons_y)}
                             }});
  }
  void restart() override {}
  void stepFwd() override {
    ++this->m_timestep;
    for (int j{0}; j < this->m_sy; ++j) {
      for (int i{0}; i < this->m_sx; ++i) {
        this->ex.set(i * this->m_sy + j, this->ex.get(i * this->m_sy + j) + 0.001f);
        this->bx.set(i * this->m_sy + j, this->bx.get(i * this->m_sy + j) + 0.001f);
      }
    }
  }
  void stepBwd() override {
    --this->m_timestep;
    for (int j{0}; j < this->m_sy; ++j) {
      for (int i{0}; i < this->m_sx; ++i) {
        this->ex.set(i * this->m_sy + j, this->ex.get(i * this->m_sy + j) - 0.001f);
        this->bx.set(i * this->m_sy + j, this->bx.get(i * this->m_sy + j) - 0.001f);
      }
    }
  }
};

auto main() -> int {
  try {
    FakeSimulation sim(100, 150);
    sim.setData();

    nttiny::Visualization<float> vis;
    vis.setTPSLimit(30.0f);
    vis.bindSimulation(&sim);
    vis.loop();
  }
  catch (std::exception& err) {
    std::cerr << err.what();
    return -1;
  }
  return 0;
}
