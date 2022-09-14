#include "nttiny/defs.h"
#include "nttiny/vis.h"
#include "nttiny/api.h"

#include <iostream>
#include <string>
#include <stdexcept>

/* -------------------------------------------------------------------------- */
/*                         simple 2D cartesian fields                         */
/* -------------------------------------------------------------------------- */
struct Example1 : public nttiny::SimulationAPI<float, 2> {
  float* ex;
  float* bz;

  Example1(int sx1, int sx2) : nttiny::SimulationAPI<float, 2>{} {
    this->m_global_grid
        = nttiny::Grid<float, 2>{nttiny::Coord::Cartesian, std::array<int, 2>{sx1, sx2}};
    for (int i{0}; i <= sx1; ++i) {
      this->m_global_grid.m_xi[0][i] = -1.0f + 2.0f * (float)(i) / (float)(sx1);
    }
    for (int j{0}; j <= sx2; ++j) {
      this->m_global_grid.m_xi[1][j] = -3.0f + 6.0f * (float)(j) / (float)(sx2);
    }
    this->fields.insert(std::make_pair("ex", ex));
    this->fields.insert(std::make_pair("bz", bz));
    this->m_timestep = 0;
    this->m_time = 0.0;
  }

  void setData() override {
    const auto nx1{this->m_global_grid.m_size[0]};
    const auto nx2{this->m_global_grid.m_size[1]};
    auto f_sx{(float)(nx1)};
    auto f_sy{(float)(nx2)};
    for (int i{0}; i < nx1; ++i) {
      auto f_i{(float)(i)};
      for (int j{0}; j < nx2; ++j) {
        auto f_j{(float)(j)};
        const auto idx{j * nx1 + i};
        ex[idx] = 0.5f * (f_i / f_sx);
        bz[idx] = (f_i / f_sx) * (f_j / f_sy);
      }
    }
  }
  void restart() override {}
  void stepFwd() override {
    ++this->m_timestep;
    ++this->m_time;
    const auto nx1{this->m_global_grid.m_size[0]};
    const auto nx2{this->m_global_grid.m_size[1]};
    for (int j{0}; j < nx2; ++j) {
      for (int i{0}; i < nx1; ++i) {
        const auto idx{j * nx1 + i};
        ex[idx] += 0.001f;
        bz[idx] += 0.001f;
      }
    }
  }
  void stepBwd() override {
    --this->m_timestep;
    --this->m_time;
    const auto nx1{this->m_global_grid.m_size[0]};
    const auto nx2{this->m_global_grid.m_size[1]};
    for (int j{0}; j < nx2; ++j) {
      for (int i{0}; i < nx1; ++i) {
        const auto idx{j * nx1 + i};
        ex[idx] -= 0.001f;
        bz[idx] -= 0.001f;
      }
    }
  }
  void customAnnotatePcolor2d() {
    setData();
    this->m_time = 0.0;
    this->m_timestep = 0;
  }
};

auto main() -> int {
  try {
    Example1 sim(10, 50);
    sim.setData();

    nttiny::Visualization<float, 2> vis;
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
