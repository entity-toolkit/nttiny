#include "defs.h"
#include "window.h"
#include "sim.h"

#include <plog/Log.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

Window::Window(int width, int height, const std::string &name, int swapInterval,
               bool isFullscreen)
    : m_winWidth(width), m_winHeight(height) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  m_win = glfwCreateWindow(m_winWidth, m_winHeight, name.c_str(),
                           isFullscreen ? glfwGetPrimaryMonitor() : nullptr,
                           nullptr);
  glfwSetWindowAspectRatio(m_win, m_winWidth, m_winHeight);
  if (m_win == nullptr) {
    PLOGE << "Failed to open window.";
  }

  glfwMakeContextCurrent(m_win);

  glfwSwapInterval(swapInterval);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    PLOGE << "Failed to load GLAD.";
  }
}

Window::~Window() { glfwTerminate(); }

void Window::use(Simulation *sim) { processInput(sim); }

void Window::unuse() {
  glfwSwapBuffers(m_win);
  glfwPollEvents();
}

void Window::processInput(Simulation *sim) {
  if (glfwGetKey(m_win, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(m_win, true);
  }
  if (glfwGetKey(m_win, GLFW_KEY_F1) == GLFW_PRESS) {
    glfwSetInputMode(m_win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }
  if (glfwGetKey(m_win, GLFW_KEY_F2) == GLFW_PRESS) {
    glfwSetInputMode(m_win, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  }
  if (glfwGetKey(m_win, GLFW_KEY_F3) == GLFW_PRESS) {
    glfwSetInputMode(m_win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }

  static bool pressing_spacebar = false;
  if (glfwGetKey(m_win, GLFW_KEY_SPACE) == GLFW_PRESS) {
    pressing_spacebar = true;
  }
  if (glfwGetKey(m_win, GLFW_KEY_SPACE) == GLFW_RELEASE &&
      (pressing_spacebar)) {
    pressing_spacebar = false;
    sim->playToggle();
  }

  if (glfwGetKey(m_win, GLFW_KEY_LEFT) == GLFW_PRESS) {
    sim->stepBwd();
  }
  if (glfwGetKey(m_win, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    sim->stepFwd();
  }

  static bool pressing_left = false;
  if (glfwGetKey(m_win, GLFW_KEY_COMMA) == GLFW_PRESS) {
    pressing_left = true;
  }
  if (glfwGetKey(m_win, GLFW_KEY_COMMA) == GLFW_RELEASE && (pressing_left)) {
    pressing_left = false;
    sim->stepBwd();
  }
  static bool pressing_right = false;
  if (glfwGetKey(m_win, GLFW_KEY_PERIOD) == GLFW_PRESS) {
    pressing_right = true;
  }
  if (glfwGetKey(m_win, GLFW_KEY_PERIOD) == GLFW_RELEASE && (pressing_right)) {
    pressing_right = false;
    sim->stepFwd();
  }
}
