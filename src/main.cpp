#include "defs.h"

#include "sim.h"

#include "window.h"
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

  int win_width{800}, win_height{480};

  FakeSimulation m_fakesim(100, 60, 20);
  m_fakesim.setData();

  Window m_window{Window(win_width, win_height, "Nttiny", 0, false)};

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
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

  // Menu m_menu{Menu(m_window.get_window(), &m_fakesim, &m_colormap)};

  // double timer{glfwGetTime()};
  double hard_limit{glfwGetTime()};
  while (!m_window.windowShouldClose()) {
    if (glfwGetTime() >= hard_limit + 1.0 / HARD_LIMIT_FPS) {
      m_window.use(&m_fakesim);

      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      // ImGui::Begin("My Window");
      if (ImPlot::BeginPlot("My Plot")) {
        // ImPlot::PlotBars("My Bar Plot", bar_data, 11);
        ImPlot::PlotLine("My Line Plot", xs1, ys1, 101);
        // ...
        ImPlot::EndPlot();
      }
      // ImGui::End();

      ImGui::Render();

      // m_menu.use();

      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      m_window.unuse();

      m_fakesim.updateData();
      hard_limit = glfwGetTime();

      // if (glfwGetTime() >=
          // timer + 1.0 / static_cast<double>(m_fakesim.get_steps_per_second())) {
        // timer = glfwGetTime();
      // }
    }
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImPlot::DestroyContext();
  ImGui::DestroyContext();
  return 0;
}
