#ifndef NTTINY_VIS_H
#define NTTINY_VIS_H

#include "window.h"
#include "api.h"
#include "plots.h"

#include <plog/Log.h>
#include <plog/Init.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/ColorConsoleAppender.h>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <implot/implot.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <memory>
#include <string>
#include <vector>

namespace nttiny {

template <class T, ushort D>
class Visualization {
  int m_id{0};
  plog::ColorConsoleAppender<plog::TxtFormatter> m_console_appender;
  std::vector<std::unique_ptr<Ax<T, D>>> m_plots;
  ImPlotRect SharedAxes{0, 1, 0, 1};
  bool m_save_image{false};
  bool m_save_video{false}, m_save_videoframe{false};
  int m_save_videoframe_count{0};
  bool m_collapsed_controls{false};

protected:
  SimulationAPI<T, D>* m_sim;
  std::unique_ptr<Window> m_window;
  int m_win_width, m_win_height;
  bool m_resizable;

public:
  Visualization(float scale = 1.0f,
                int win_width = 2560,
                int win_height = 1440,
                bool resizable = true);
  ~Visualization();
  void bindSimulation();
  void bindSimulation(SimulationAPI<T, D>* sim);

  void drawMainMenuBar();
  void drawControls();
  void processControllerInput();
  void loop();
  void addPcolor2d();
  void addScatter2d();
  void addTimePlot();

  void loadState();
  void saveState();

  UISettings UI_Settings;
};

} // namespace nttiny

#endif
