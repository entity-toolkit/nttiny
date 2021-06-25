#include "defs.h"

#include "sim.h"

#include "window.h"
#include "mpl.h"
// #include "menu.h"

#include <plog/Log.h>
#include <plog/Init.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/ColorConsoleAppender.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <implot.h>

// TODO: simulation controller
// TODO: abstractify to use with `ntt`

// TODO: issue with array being rendered reversed in y

// USAGE PLANS:
// Simulation mySimulation(...);
//
// main() {
//   mySimulation.initialize();
//   // Simulation.loop();
//   Interactive.loop(&mySimulation);
//   mySimulation.finalize();
// }

auto main() -> int {
  static plog::ColorConsoleAppender<plog::TxtFormatter> console_appender;
  plog::Severity max_severity;
#ifdef VERBOSE
  max_severity = plog::verbose;
#elif DEBUG
  max_severity = plog::debug;
#else
  max_severity = plog::warning;
#endif
  plog::init(max_severity, &console_appender);

  int win_width{1920}, win_height{1080};

  FakeSimulation m_fakesim(100, 60, 20);
  m_fakesim.setData();

  Window m_window{Window(win_width, win_height, "Nttiny", 0, false)};

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  (void)io;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(m_window.get_window(), true);
  ImGui_ImplOpenGL3_Init("#version 150");

  double xs1[101], ys1[101], ys2[101], ys3[101];
  for (int i{0}; i < 101; ++i) {
    xs1[i] = static_cast<double>(i);
    ys1[i] = xs1[i] * xs1[i];
    ys2[i] = xs1[i] * 0.5;
    ys3[i] = xs1[i] * xs1[i] * xs1[i];
  }

  float values1[100][150];
  for (int j{0}; j < 100; ++j) {
    for (int i{0}; i < 150; ++i) {
      values1[j][i] = static_cast<float>(i * i - j * j);
    }
  }

  float values2[60][40];
  for (int j{0}; j < 60; ++j) {
    for (int i{0}; i < 40; ++i) {
      values2[j][i] = static_cast<float>(60 - j);
    }
  }

  Pcolor2d plot2d_1(-1e4, 1e4, {0, 100}, {0, 150});
  Pcolor2d plot2d_2(0.0f, 100.0f, {0, 60}, {0, 40});
  Plot plot_1({0, 101}, {0, 100}, 1.5);
  // Menu m_menu{Menu(m_window.get_window(), &m_fakesim, &m_colormap)};

  // double timer{glfwGetTime()};
  double hard_limit{glfwGetTime()};
  while (!m_window.windowShouldClose()) {
    // if (glfwGetTime() >= hard_limit + 1.0 / HARD_LIMIT_FPS) {
    m_window.use(&m_fakesim);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowDemoWindow();
    plot2d_1.draw(values1[0], 100, 150);
    plot2d_2.draw(values2[0], 60, 40);
    plot_1.draw(xs1, ys1, 101);

    ImGui::Render();

    // m_menu.use();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    m_window.unuse();

    // m_fakesim.updateData();
    // hard_limit = glfwGetTime();

    // if (glfwGetTime() >=
    // timer + 1.0 / static_cast<double>(m_fakesim.get_steps_per_second())) {
    // timer = glfwGetTime();
    // }
    // }
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImPlot::DestroyContext();
  ImGui::DestroyContext();
  return 0;
}
