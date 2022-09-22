#include "nttiny/defs.h"
#include "nttiny/vis.h"
#include "nttiny/api.h"

#include <stdlib.h>
#include <iostream>
#include <string>
#include <stdexcept>

void InitRandomSeed() { srand(time(nullptr)); }

template <typename T>
auto random() -> T {
  return static_cast<T>(rand()) / static_cast<T>(RAND_MAX);
}

template <typename T>
auto SphToCart(const T& r, const T& theta) -> std::pair<T, T> {
  return {r * std::sin(theta), r * std::cos(theta)};
}

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
  void customAnnotatePcolor2d() override {}
};

/* -------------------------------------------------------------------------- */
/*                     2D logR spherical grid + particles                     */
/* -------------------------------------------------------------------------- */
struct Example3 : public nttiny::SimulationAPI<float, 2> {
  const float m_rmin, m_rmax;

  Example3(int sx1, int sx2, const float& rmin, const float& rmax)
      : nttiny::SimulationAPI<float, 2>{nttiny::Coord::Spherical, {sx1, sx2}, 2},
        m_rmin{rmin},
        m_rmax{rmax} {
    const auto nx1{this->m_global_grid.m_size[0] + this->m_global_grid.m_ngh * 2};
    const auto nx2{this->m_global_grid.m_size[1] + this->m_global_grid.m_ngh * 2};
    for (int i{0}; i <= sx1; ++i) {
      this->m_global_grid.m_xi[0][i]
          = std::log(rmin) + (std::log(rmax) - std::log(rmin)) * (float)(i) / (float)(sx1);
      this->m_global_grid.m_xi[0][i] = std::exp(this->m_global_grid.m_xi[0][i]);
    }
    for (int j{0}; j <= sx2; ++j) {
      this->m_global_grid.m_xi[1][j] = 0.0f + M_PI * (float)(j) / (float)(sx2);
    }
    this->m_global_grid.ExtendGridWithGhosts();
    this->fields.insert({"er", new float[nx1 * nx2]});
    this->fields.insert({"bphi", new float[nx1 * nx2]});
    this->fields.insert({"r", new float[nx1 * nx2]});
    this->fields.insert({"theta", new float[nx1 * nx2]});

    const auto nprtl = 100;

    this->particles.insert({"e-", {nprtl, {new float[nprtl], new float[nprtl]}}});
    this->particles.insert({"e+", {nprtl, {new float[nprtl], new float[nprtl]}}});

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

    for (auto species : this->particles) {
      auto nprtl = species.second.first;
      auto x1 = species.second.second[0];
      auto x2 = species.second.second[1];
      for (int p{0}; p < nprtl; ++p) {
        auto xy = SphToCart<float>(random<float>() * (m_rmax - m_rmin) + m_rmin,
                                   random<float>() * M_PI);
        x1[p] = xy.first;
        x2[p] = xy.second;
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
  void customAnnotatePcolor2d() override {}
};

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
  void customAnnotatePcolor2d() override {}
};

auto main(int argc, char** argv) -> int {
  float scale = 2.0f;
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
    // Example1 sim(32, 16);
    // Example2 sim(32, 16, 1.0f, 10.0f);
    Example3 sim(256, 256, 1.0f, 50.0f);
    // Example4 sim(256, 128);
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