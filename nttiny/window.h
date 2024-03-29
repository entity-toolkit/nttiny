#ifndef NTTINY_WINDOW_H
#define NTTINY_WINDOW_H

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <string>

namespace nttiny {
  class Window {
  private:
    GLFWwindow* m_win;
    int         m_winWidth, m_winHeight;
    const char* m_glsl_version = "#version 150";

  public:
    Window(int                width,
           int                height,
           const std::string& name,
           int                swapInterval,
           bool               isResizable = true);
    ~Window() = default;
    void               processInput();
    void               unuse();
    [[nodiscard]] auto windowShouldClose() const -> int {
      return glfwWindowShouldClose(m_win);
    }
    [[nodiscard]] auto get_window() const -> GLFWwindow* {
      return m_win;
    }
    [[nodiscard]] auto get_glsl_version() const -> const char* {
      return m_glsl_version;
    }
  };
}    // namespace nttiny

#endif
