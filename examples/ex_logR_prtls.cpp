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

template <typename T>
auto SphToCart(const T& r, const T& theta) -> std::pair<T, T> {
  return {r * std::sin(theta), r * std::cos(theta)};
}

/* -------------------------------------------------------------------------- */
/*                2D logR spherical grid + particles + buffers                */
/* -------------------------------------------------------------------------- */
struct Example3 : public nttiny::SimulationAPI<float, 2> {
  const float m_rmin, m_rmax;

  Example3(int sx1, int sx2, const float& rmin, const float& rmax)
      : nttiny::SimulationAPI<float, 2>{"Ex3", nttiny::Coord::Spherical, {sx1, sx2}, 2},
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
    this->fields.insert({"a_const", new float[nx1 * nx2]});
    this->fields.insert({"er", new float[nx1 * nx2]});
    this->fields.insert({"bphi", new float[nx1 * nx2]});
    this->fields.insert({"r", new float[nx1 * nx2]});
    this->fields.insert({"theta", new float[nx1 * nx2]});

    const auto nprtl = 100;

    this->particles.insert({"e-", {nprtl, {new float[nprtl], new float[nprtl]}}});
    this->particles.insert({"e+", {nprtl, {new float[nprtl], new float[nprtl]}}});

    nttiny::ScrollingBuffer b1, b2, b3;
    this->buffers.insert({"sum_er", std::move(b1)});
    this->buffers.insert({"mean_bphi", std::move(b2)});
    this->buffers.insert({"er(20, 20)", std::move(b3)});

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
          (this->fields)["a_const"][Index(i, j)] = 5.0f;
          (this->fields)["r"][Index(i, j)] = Xi(i, 0);
          (this->fields)["theta"][Index(i, j)] = Xi(j, 1);
        } else {
          (this->fields)["er"][Index(i, j)] = -1.0f;
          (this->fields)["bphi"][Index(i, j)] = -1.0f;
          (this->fields)["a_const"][Index(i, j)] = 5.0f;
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
  void setData() override {
    const auto sx1{this->m_global_grid.m_size[0]};
    const auto sx2{this->m_global_grid.m_size[1]};
    auto sum_er{0.0f}, mean_bphi{0.0f};
    for (int j{0}; j < sx2; ++j) {
      for (int i{0}; i < sx1; ++i) {
        sum_er += (this->fields)["er"][Index(i, j)];
        mean_bphi += (this->fields)["bphi"][Index(i, j)];
      }
    }
    this->buffers["sum_er"].AddPoint(this->m_time, sum_er);
    this->buffers["mean_bphi"].AddPoint(this->m_time, -mean_bphi / (float)(sx1 * sx2));
    this->buffers["er(20, 20)"].AddPoint(this->m_time, (this->fields)["er"][Index(20, 20)]);
  }
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
  void customAnnotatePcolor2d(const nttiny::UISettings& ui_settings) override {
    nttiny::tools::drawCircle({0.0f, 0.0f}, 5.0f, {0.0f, M_PI}, 128, ui_settings.OutlineColor);
  }
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
    Example3 sim(256, 256, 1.0f, 50.0f);
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