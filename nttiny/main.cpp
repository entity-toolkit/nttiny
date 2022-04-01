#include "nttiny/vis.h"
#include "nttiny/api.h"

#include <iostream>
#include <string>
#include <stdexcept>

class FieldVisualization : public nttiny::SimulationAPI<float> {
public:
  int nx1, nx2;
  nttiny::Data<float> ex;
  nttiny::Data<float> bx;

  // nttiny::Data<float> electrons_x;
  // nttiny::Data<float> electrons_y;
  // nttiny::Data<float> positrons_x;
  // nttiny::Data<float> positrons_y;

  FieldVisualization(int sx1, int sx2)
      // : nttiny::SimulationAPI<float>{"cartesian"},
      : nttiny::SimulationAPI<float>{"polar"}, nx1(sx1), nx2(sx2), ex{sx1, sx2}, bx{sx1, sx2} {
    m_x1x2_extent[0] = 1.0f;
    m_x1x2_extent[1] = 5.0f;
    m_x1x2_extent[2] = 0.0f;
    m_x1x2_extent[3] = M_PI;

    for (int i{0}; i <= sx1; ++i) {
      ex.grid_x1[i] = m_x1x2_extent[0]
                    + (m_x1x2_extent[1] - m_x1x2_extent[0])
                          * (exp((double)(i) / (double)(sx1)) - 1.0) / (exp(1.0) - 1.0);
      bx.grid_x1[i] = ex.grid_x1[i];
    }
    for (int j{0}; j <= sx2; ++j) {
      ex.grid_x2[j]
          = m_x1x2_extent[2] + (m_x1x2_extent[3] - m_x1x2_extent[2]) * (double)(j) / (double)(sx2);
      bx.grid_x2[j] = ex.grid_x2[j];
    }

    this->fields.insert({{"ex", &(this->ex)}, {"bx", &(this->bx)}});
  }
  // this->ex.allocate(sx * sy);
  // this->bx.allocate(sx * sy);
  // this->ex.set_size(0, sx);
  // this->ex.set_size(1, sy);
  // this->bx.set_size(0, sx);
  // this->bx.set_size(1, sy);
  // this->ex.set_dimension(2);
  // this->bx.set_dimension(2);

  // this->electrons_x.allocate(1000);
  // this->electrons_y.allocate(1000);
  // this->positrons_x.allocate(1000);
  // this->positrons_y.allocate(1000);
  //
  // this->electrons_x.set_size(0, 1000);
  // this->electrons_y.set_size(0, 1000);
  // this->positrons_x.set_size(0, 1000);
  // this->positrons_y.set_size(0, 1000);
  // }
  void setData() override {
    this->m_timestep = 0;
    auto f_sx{(float)(this->nx1)};
    auto f_sy{(float)(this->nx2)};
    for (int i{0}; i < this->nx1; ++i) {
      auto f_i{(float)(i)};
      for (int j{0}; j < this->nx2; ++j) {
        auto f_j{(float)(j)};
        this->ex.set(i, j, 0.5f * (f_i / f_sx));
        this->bx.set(i, j, (f_i / f_sx) * (f_j / f_sy));
      }
    }
    // for (int i = 0; i < 1000; ++i) {
    //   this->electrons_x.set(i, m_x1x2_extent[1] * i / 1000.0);
    //   this->electrons_y.set(i, m_x1x2_extent[3] * i / 1000.0);
    //   this->positrons_x.set(i, 0.1f + m_x1x2_extent[1] * i / 1000.0);
    //   this->positrons_y.set(i, 0.1f + m_x1x2_extent[3] * i / 1000.0);
    // }
    // this->particles.insert({{"electrons",
    //                             {&(this->electrons_x),
    //                              &(this->electrons_y)}
    //                          },{
    //                          "positrons",
    //                             {&(this->positrons_x),
    //                              &(this->positrons_y)}
    //                          }});
  }
  void restart() override {}
  void stepFwd() override {
    ++this->m_timestep;
    for (int j{0}; j < this->nx2; ++j) {
      for (int i{0}; i < this->nx1; ++i) {
        this->ex.set(i, j, this->ex.get(i, j) + 0.001f);
        this->bx.set(i, j, this->bx.get(i, j) + 0.001f);
      }
    }
  }
  void stepBwd() override {
    --this->m_timestep;
    for (int j{0}; j < this->nx2; ++j) {
      for (int i{0}; i < this->nx1; ++i) {
        this->ex.set(i, j, this->ex.get(i, j) - 0.001f);
        this->bx.set(i, j, this->bx.get(i, j) - 0.001f);
      }
    }
  }
};

auto main() -> int {
  try {
    FieldVisualization sim(10, 50);
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
