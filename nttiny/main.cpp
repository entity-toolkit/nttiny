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
      : nttiny::SimulationAPI<float, 2>{nttiny::Coord::Cartesian, {sx1, sx2}, 2} {
    const auto nx1{this->m_global_grid.m_size[0] + this->m_global_grid.m_ngh * 2};
    const auto nx2{this->m_global_grid.m_size[1] + this->m_global_grid.m_ngh * 2};
    for (int i{0}; i <= sx1; ++i) {
      this->m_global_grid.m_xi[0][i] = -3.0f + 6.0f * (float)(i) / (float)(sx1);
    }
    for (int j{0}; j <= sx2; ++j) {
      this->m_global_grid.m_xi[1][j] = -1.5f + 3.0f * (float)(j) / (float)(sx2);
    }
    this->fields.insert({"ex", new float[nx1 * nx2]});
    this->fields.insert({"bz", new float[nx1 * nx2]});
    this->fields.insert({"xx", new float[nx1 * nx2]});
    this->fields.insert({"yy", new float[nx1 * nx2]});
    this->m_timestep = 0;
    this->m_time = 0.0;
  }

  void setData() override {
    const auto sx1{this->m_global_grid.m_size[0]};
    const auto sx2{this->m_global_grid.m_size[1]};
    const auto ngh{m_global_grid.m_ngh};
    auto f_sx{(float)(sx1)};
    auto f_sy{(float)(sx2)};

    for (int j{-ngh}; j < sx2 + ngh; ++j) {
      auto f_j{(float)(j)};
      for (int i{-ngh}; i < sx1 + ngh; ++i) {
        auto f_i{(float)(i)};
        if (i >= 0 && i < sx1 && j >= 0 && j < sx2) {
          (this->fields)["ex"][Index(i, j)] = 0.5f * (f_i / f_sx);
          (this->fields)["bz"][Index(i, j)] = (f_i / f_sx) * (f_j / f_sy);
          (this->fields)["xx"][Index(i, j)] = Xi(i, 0);
          (this->fields)["yy"][Index(i, j)] = Xi(j, 1);
        } else {
          (this->fields)["ex"][Index(i, j)] = -1.0f;
          (this->fields)["bz"][Index(i, j)] = -1.0f;
          (this->fields)["xx"][Index(i, j)] = -1.0f;
          (this->fields)["yy"][Index(i, j)] = -1.0f;
        }
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
    const auto sx1{this->m_global_grid.m_size[0]};
    const auto sx2{this->m_global_grid.m_size[1]};
    for (int j{0}; j < sx2; ++j) {
      for (int i{0}; i < sx1; ++i) {
        (this->fields)["ex"][Index(i, j)] += 0.001f;
        (this->fields)["bz"][Index(i, j)] += 0.001f;
      }
    }
  }
  void stepBwd() override {
    --this->m_timestep;
    --this->m_time;
    const auto sx1{this->m_global_grid.m_size[0]};
    const auto sx2{this->m_global_grid.m_size[1]};
    for (int j{0}; j < sx2; ++j) {
      for (int i{0}; i < sx1; ++i) {
        (this->fields)["ex"][Index(i, j)] -= 0.001f;
        (this->fields)["bz"][Index(i, j)] -= 0.001f;
      }
    }
  }
  void customAnnotatePcolor2d() override {}
};

/* -------------------------------------------------------------------------- */
/*                          2D uniform spherical grid                         */
/* -------------------------------------------------------------------------- */
struct Example2 : public nttiny::SimulationAPI<float, 2> {
  Example2(int sx1, int sx2, const float& rmin, const float& rmax)
      : nttiny::SimulationAPI<float, 2>{nttiny::Coord::Spherical, {sx1, sx2}, 2} {
    const auto nx1{this->m_global_grid.m_size[0] + this->m_global_grid.m_ngh * 2};
    const auto nx2{this->m_global_grid.m_size[1] + this->m_global_grid.m_ngh * 2};
    for (int i{0}; i <= sx1; ++i) {
      this->m_global_grid.m_xi[0][i] = rmin + (rmax - rmin) * (float)(i) / (float)(sx1);
    }
    for (int j{0}; j <= sx2; ++j) {
      this->m_global_grid.m_xi[1][j] = 0.0f + M_PI * (float)(j) / (float)(sx2);
    }
    this->fields.insert({"er", new float[nx1 * nx2]});
    this->fields.insert({"bphi", new float[nx1 * nx2]});
    this->fields.insert({"r", new float[nx1 * nx2]});
    this->fields.insert({"theta", new float[nx1 * nx2]});
    this->m_timestep = 0;
    this->m_time = 0.0;
  }

  void setData() override {
    const auto sx1{this->m_global_grid.m_size[0]};
    const auto sx2{this->m_global_grid.m_size[1]};
    const auto ngh{m_global_grid.m_ngh};
    auto f_sx{(float)(sx1)};
    auto f_sy{(float)(sx2)};

    for (int j{-ngh}; j < sx2 + ngh; ++j) {
      auto f_j{(float)(j)};
      for (int i{-ngh}; i < sx1 + ngh; ++i) {
        auto f_i{(float)(i)};
        if (i >= 0 && i < sx1 && j >= 0 && j < sx2) {
          (this->fields)["er"][Index(i, j)] = 0.5f * (f_i / f_sx);
          (this->fields)["bphi"][Index(i, j)] = (f_i / f_sx) * (f_j / f_sy);
          (this->fields)["r"][Index(i, j)] = Xi(i, 0);
          (this->fields)["theta"][Index(i, j)] = Xi(j, 1);
        } else {
          (this->fields)["er"][Index(i, j)] = -1.0f;
          (this->fields)["bphi"][Index(i, j)] = -1.0f;
          (this->fields)["r"][Index(i, j)] = -1.0f;
          (this->fields)["theta"][Index(i, j)] = -1.0f;
        }
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
    const auto sx1{this->m_global_grid.m_size[0]};
    const auto sx2{this->m_global_grid.m_size[1]};
    for (int j{0}; j < sx2; ++j) {
      for (int i{0}; i < sx1; ++i) {
        (this->fields)["er"][Index(i, j)] += 0.001f;
        (this->fields)["bphi"][Index(i, j)] += 0.001f;
      }
    }
  }
  void stepBwd() override {
    --this->m_timestep;
    --this->m_time;
    const auto sx1{this->m_global_grid.m_size[0]};
    const auto sx2{this->m_global_grid.m_size[1]};
    for (int j{0}; j < sx2; ++j) {
      for (int i{0}; i < sx1; ++i) {
        (this->fields)["er"][Index(i, j)] -= 0.001f;
        (this->fields)["bphi"][Index(i, j)] -= 0.001f;
      }
    }
  }
  void customAnnotatePcolor2d() override {}
};

auto main() -> int {
  try {
    // Example1 sim(32, 16);
    Example2 sim(32, 16, 1.0f, 10.0f);
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
