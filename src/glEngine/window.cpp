#include "defs.h"
#include "renderer.h"
#include "window.h"
#include "program.h"
#include "shaders.h"

#include <fmt/core.h>
#include <plog/Log.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>

static void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
  UNUSED(window); // to avoid the "unused variable" warning
  glViewport(0, 0, width, height);
}

void Window::initialize() {
  if (!GLFW_INITIALIZED) {
    PLOGE << "`glfw` not initialized";
    return;
  }
  // TODO: get variables for dimensions and window title
  this->window = glfwCreateWindow(800, 600, "testogl", nullptr, nullptr);
  if (this->window == nullptr) {
    PLOGE << "unable to create a window";
    this->finalize();
    return;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    PLOGE << "unable to load `glad`";
    return;
  }
  PLOGV << "window initialized";
}

void Window::createProgram(const std::vector<const char *> &shaders) {
  if (!GLFW_INITIALIZED) {
    PLOGE << "`glfw` not initialized";
    return;
  }
  Program program;
  program.create();
  for (auto &shader : shaders) {
    program.attachShader(shader);
  }
  program.link();
  (this->programs).push_back(program.ind);
}

void Window::finalize() {
  if (!GLFW_INITIALIZED) {
    PLOGE << "`glfw` not initialized";
    return;
  }
  for (auto &p : (this->programs)) {
    glDeleteProgram(p);
    PLOGV << fmt::format("program {} deleted", p);
  }
}
