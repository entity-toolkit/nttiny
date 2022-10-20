#include "defs.h"
#include "vis.h"
#include "api.h"
#include "tools.h"

#include <stdlib.h>
#include <iostream>
#include <string>
#include <stdexcept>

void InitRandomSeed() { srand(time(nullptr)); }

template <typename T>
auto random() -> T {
  return static_cast<T>(rand()) / static_cast<T>(RAND_MAX);
}

/* -------------------------------------------------------------------------- */
/*                      2D cartesian + particles (double)                     */
/* -------------------------------------------------------------------------- */
struct Example4 : public nttiny::SimulationAPI<double, 2> {
  Example4(int sx1, int sx2)
      : nttiny::SimulationAPI<double, 2>{nttiny::Coord::Cartesian, {sx1, sx2}, 2} {
    const auto nx1{this->m_global_grid.m_size[0] + this->m_global_grid.m_ngh * 2};
    const auto nx2{this->m_global_grid.m_size[1] + this->m_global_grid.m_ngh * 2};
    for (int i{0}; i <= sx1; ++i) {
      this->m_global_grid.m_xi[0][i] = -3.0f + 6.0f * (double)(i) / (double)(sx1);
    }
    for (int j{0}; j <= sx2; ++j) {
      this->m_global_grid.m_xi[1][j] = -1.5f + 3.0f * (double)(j) / (double)(sx2);
    }
    this->fields.insert({"ex", new double[nx1 * nx2]});
    this->fields.insert({"bz", new double[nx1 * nx2]});
    this->fields.insert({"xx", new double[nx1 * nx2]});
    this->fields.insert({"yy", new double[nx1 * nx2]});

    const auto nprtl = 100;

    this->particles.insert({"e-", {nprtl, {new double[nprtl], new double[nprtl]}}});
    this->particles.insert({"e+", {nprtl, {new double[nprtl], new double[nprtl]}}});

    this->m_timestep = 0;
    this->m_time = 0.0;
    initData();
  }

  void initData() {
    const auto sx1{this->m_global_grid.m_size[0]};
    const auto sx2{this->m_global_grid.m_size[1]};
    const auto ngh{m_global_grid.m_ngh};
    auto f_sx{(float)(sx1)};
    auto f_sy{(float)(sx2)};

    auto x1min = this->m_global_grid.m_xi[0][0];
    auto x1max = this->m_global_grid.m_xi[0][sx1];
    auto x2min = this->m_global_grid.m_xi[1][0];
    auto x2max = this->m_global_grid.m_xi[1][sx2];

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
    for (auto species : this->particles) {
      auto nprtl = species.second.first;
      auto x1 = species.second.second[0];
      auto x2 = species.second.second[1];
      for (int p{0}; p < nprtl; ++p) {
        x1[p] = random<double>() * (x1max - x1min) + x1min,
        x2[p] = random<double>() * (x2max - x2min) + x2min;
      }
    }
  }
  void setData() override {}
  void restart() override {
    initData();
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
  void customAnnotatePcolor2d(const nttiny::UISettings&) override {}
};

auto main(int argc, char** argv) -> int {
  float scale = 1.0f;
  if (argc == 3) {
    if (std::string(argv[1]) == "-scale" || std::string(argv[1]) == "-s") {
      scale = std::stof(argv[2]);
      std::cout << "Using UI scale: " << scale << std::endl;
    } else {
      std::cout << "Using default UI scale: " << scale
                << "\nTo override this setting rerun with `-scale <float>` or `-s <float>`\n";
    }
  }
  try {
    Example4 sim(256, 128);
    sim.setData();

    nttiny::Visualization<double, 2> vis{scale};
    vis.bindSimulation(&sim);
    vis.loop();
  }
  catch (std::exception& err) {
    std::cerr << err.what();
    return -1;
  }
  return 0;
}