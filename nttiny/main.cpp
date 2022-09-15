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
  Example1(int sx1, int sx2)
      : nttiny::SimulationAPI<float, 2>{nttiny::Coord::Cartesian, {sx1, sx2}} {
    for (int i{0}; i <= sx1; ++i) {
      this->m_global_grid.m_xi[0][i] = -3.0f + 6.0f * (float)(i) / (float)(sx1);
    }
    for (int j{0}; j <= sx2; ++j) {
      this->m_global_grid.m_xi[1][j] = -1.5f + 3.0f * (float)(j) / (float)(sx2);
    }
    this->fields.insert({"ex", new float[sx1 * sx2]});
    this->fields.insert({"bz", new float[sx1 * sx2]});
    this->fields.insert({"xx", new float[sx1 * sx2]});
    this->fields.insert({"yy", new float[sx1 * sx2]});
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
        (this->fields)["ex"][Index(i, j)] = 0.5f * (f_i / f_sx);
        (this->fields)["bz"][Index(i, j)] = (f_i / f_sx) * (f_j / f_sy);
        (this->fields)["xx"][Index(i, j)] = Xi(i, 0);
        (this->fields)["yy"][Index(i, j)] = Xi(j, 1);
      }
    }
  }
  void restart() override {
    setData();
    this->m_time = 0.0;
    this->m_timestep = 0;
  }
  void stepFwd() override {
    ++this->m_timestep;
    ++this->m_time;
    const auto nx1{this->m_global_grid.m_size[0]};
    const auto nx2{this->m_global_grid.m_size[1]};
    for (int j{0}; j < nx2; ++j) {
      for (int i{0}; i < nx1; ++i) {
        const auto idx{i * nx2 + j};
        (this->fields)["ex"][idx] += 0.001f;
        (this->fields)["bz"][idx] += 0.001f;
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
        const auto idx{i * nx2 + j};
        (this->fields)["ex"][idx] -= 0.001f;
        (this->fields)["bz"][idx] -= 0.001f;
      }
    }
  }
  void customAnnotatePcolor2d() {}
};

auto main() -> int {
  try {
    Example1 sim(32, 16);
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
