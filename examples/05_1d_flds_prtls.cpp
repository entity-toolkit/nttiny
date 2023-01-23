#include "api.h"
#include "defs.h"
#include "tools.h"
#include "vis.h"

#include <stdlib.h>

#include <iostream>
#include <stdexcept>
#include <string>

void InitRandomSeed() {
  srand(time(nullptr));
}

template <typename T>
auto random() -> T {
  return static_cast<T>(rand()) / static_cast<T>(RAND_MAX);
}

struct Example5 : public nttiny::SimulationAPI<float, 1> {
  Example5(int sx)
    : nttiny::SimulationAPI<float, 1> { "Ex5", nttiny::Coord::Cartesian, { sx }, 0 } {
    for (int i { 0 }; i <= sx; ++i) {
      this->m_global_grid.m_xi[0][i] = -2.0f + 4.0f * (float)(i) / (float)(sx);
    }
    this->m_global_grid.DefineCellCenters();

    this->fields.insert({ "ex", new float[sx] });
    this->fields.insert({ "bz", new float[sx] });
    this->fields.insert({ "xx", new float[sx] });

    const auto nprtl = 100;

    this->m_timestep = 0;
    this->m_time     = 0.0;
    initData();
  }

  void initData() {
    const auto sx { this->m_global_grid.m_size[0] };
    auto       f_sx { (float)(sx) };

    auto       xmin = this->m_global_grid.m_xi[0][0];
    auto       xmax = this->m_global_grid.m_xi[0][sx];

    for (int i { 0 }; i < sx; ++i) {
      auto f_i { (float)(i) };
      (this->fields)["ex"][Index(i)] = 0.5f * (f_i / f_sx);
      (this->fields)["bz"][Index(i)] = (f_i / f_sx) * (1.0 - f_i);
      (this->fields)["xx"][Index(i)] = Xi(i, 0);
    }
  }
  void setData() override {}
  void restart() override {
    initData();
    this->m_time     = 0.0;
    this->m_timestep = 0;
  }
  void stepFwd() override {
    ++this->m_timestep;
    ++this->m_time;
    const auto sx { this->m_global_grid.m_size[0] };
    for (int i { 0 }; i < sx; ++i) {
      (this->fields)["ex"][Index(i)] += 0.01f;
      (this->fields)["bz"][Index(i)] += 0.01f * ((float)i / (float)sx) * (1.0f - (float)i);
    }
  }
  void stepBwd() override {
    --this->m_timestep;
    --this->m_time;
    const auto sx { this->m_global_grid.m_size[0] };
    for (int i { 0 }; i < sx; ++i) {
      (this->fields)["ex"][Index(i)] -= 0.01f;
      (this->fields)["bz"][Index(i)] -= 0.01f * ((float)i / (float)sx) * (1.0f - (float)i);
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
    Example5 sim(256);
    sim.setData();

    nttiny::Visualization<float, 1> vis { scale };
    vis.bindSimulation(&sim);
    vis.loop();
  } catch (std::exception& err) {
    std::cerr << err.what();
    return -1;
  }
  return 0;
}