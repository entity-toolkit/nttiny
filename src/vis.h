#ifndef NTTINY_VIS_H
#define NTTINY_VIS_H

#include "window.h"
#include "plots.h"
#include "api.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <memory>
#include <string>
#include <vector>

template<class T>
class Visualization {
private:
  int m_id{0};
protected:
  SimulationAPI<T> *m_sim;
  std::unique_ptr<Window> m_window;
  int m_win_width, m_win_height;
  bool m_resizable;
  float m_fps_limit{0.0f};
  float m_tps_limit{0.0f};
  std::vector<std::unique_ptr<Ax<T>>> m_plots;
public:
  void bindSimulation(SimulationAPI<T> *sim);
  void bindSimulation();
  Visualization(int win_width=1280, int win_height=720, bool resizable=true);
  ~Visualization();
  void setFPSLimit(float fps_limit) { m_fps_limit = fps_limit; }
  void setTPSLimit(float tps_limit) { m_tps_limit = tps_limit; }
  void buildController();
  void processControllerInput();
  void loop();
  void addPcolor2d(float vmin, float vmax);
};

#endif
