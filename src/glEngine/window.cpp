#include "defs.h"
#include "window.h"
#include "shader.h"

#include <fmt/core.h>
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

  m_win = glfwCreateWindow(width, height, name.c_str(),
                           isFullscreen ? glfwGetPrimaryMonitor() : nullptr,
                           nullptr);
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

void Window::use() { processInput(); }

void Window::unuse() {
  glfwSwapBuffers(m_win);
  glfwPollEvents();
}

void Window::processInput() {
  if (glfwGetKey(m_win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(m_win, true);
  if (glfwGetKey(m_win, GLFW_KEY_F1) == GLFW_PRESS)
    glfwSetInputMode(m_win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  if (glfwGetKey(m_win, GLFW_KEY_F2) == GLFW_PRESS)
    glfwSetInputMode(m_win, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  if (glfwGetKey(m_win, GLFW_KEY_F3) == GLFW_PRESS)
    glfwSetInputMode(m_win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Window::setStandardUniforms(const Shader &shader) {
  auto curr_frame{static_cast<float>(glfwGetTime())};
  m_deltaTime = curr_frame - m_prevFrame;
  m_prevFrame = curr_frame;
  shader.setFloat("u_dtime", m_deltaTime);
  shader.setFloat("u_time", curr_frame);

  glfwGetFramebufferSize(m_win, &m_winWidth, &m_winHeight);
  shader.setVec2("u_resolution", static_cast<float>(m_winWidth),
                 static_cast<float>(m_winHeight));

  double cur_x, cur_y;
  glfwGetCursorPos(m_win, &cur_x, &cur_y);
  m_xPos = static_cast<float>(2.0 * cur_x / m_winWidth);
  m_yPos = static_cast<float>(1.0 - 2.0 * cur_y / m_winHeight);
  shader.setVec2("u_mouse", m_xPos, m_yPos);
}
