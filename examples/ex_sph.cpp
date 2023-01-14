#include "defs.h"
#include "vis.h"
#include "api.h"
#include "tools.h"

#include <stdlib.h>
#include <iostream>
#include <string>
#include <stdexcept>

/* -------------------------------------------------------------------------- */
/*                          2D uniform spherical grid                         */
/* -------------------------------------------------------------------------- */
struct Example2 : public nttiny::SimulationAPI<float, 2> {
  Example2(int sx1, int sx2, const float& rmin, const float& rmax)
      : nttiny::SimulationAPI<float, 2>{"Ex2", nttiny::Coord::Spherical, {sx1, sx2}, 2} {
    const auto nx1{this->m_global_grid.m_size[0] + this->m_global_grid.m_ngh * 2};
    const auto nx2{this->m_global_grid.m_size[1] + this->m_global_grid.m_ngh * 2};
    for (int i{0}; i <= sx1; ++i) {
      this->m_global_grid.m_xi[0][i] = rmin + (rmax - rmin) * (float)(i) / (float)(sx1);
    }
    for (int j{0}; j <= sx2; ++j) {
      this->m_global_grid.m_xi[1][j] = 0.0f + M_PI * (float)(j) / (float)(sx2);
    }
    this->m_global_grid.ExtendGridWithGhosts();
    this->fields.insert({"er", new float[nx1 * nx2]});
    this->fields.insert({"bphi", new float[nx1 * nx2]});
    this->fields.insert({"r", new float[nx1 * nx2]});
    this->fields.insert({"theta", new float[nx1 * nx2]});
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
    Example2 sim(32, 16, 1.0f, 10.0f);
    sim.setData();

    nttiny::Visualization<float, 2> vis{scale};
    vis.bindSimulation(&sim);
    vis.loop();
  }
  catch (std::exception& err) {
    std::cerr << err.what();
    return -1;
  }
  return 0;
}