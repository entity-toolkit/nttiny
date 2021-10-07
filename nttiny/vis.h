#ifndef NTTINY_VIS_H
#define NTTINY_VIS_H

#include "window.h"
#include "plots.h"
#include "api.h"

#include <plog/Log.h>
#include <plog/Init.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/ColorConsoleAppender.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <memory>
#include <string>
#include <vector>

namespace nttiny {
  template <class T> class Visualization {
  private:
    int m_id{0};
    plog::ColorConsoleAppender<plog::TxtFormatter> m_console_appender;

  protected:
    SimulationAPI<T> *m_sim;
    std::unique_ptr<Window> m_window;
    int m_win_width, m_win_height;
    bool m_resizable;
    float m_fps_limit{0.0f};
    float m_tps_limit{0.0f};
    std::vector<std::unique_ptr<Ax<T>>> m_plots;

  public:
    Visualization(int win_width = 1280, int win_height = 720,
                  bool resizable = true);
    ~Visualization();
    void bindSimulation();
    void bindSimulation(SimulationAPI<T> *sim);
    void setFPSLimit(float fps_limit) { m_fps_limit = fps_limit; }
    void setTPSLimit(float tps_limit) { m_tps_limit = tps_limit; }
    void buildController();
    void processControllerInput();
    void loop();
    void addPcolor2d(float vmin, float vmax);
    void addScatter2d();
  };
}

#endif
