#include "defs.h"
#include "window.h"
#include "program.h"
#include "shaders.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  UNUSED(window); // to avoid the "unused variable" warning 
  glViewport(0, 0, width, height);
}

void Window::initialize() {
  if (!GLFW_INITIALIZED) {
    _throwError("`glfw` not initialized");
    return;
  }
  // TODO: get variables for dimensions and window title
  this->window = glfwCreateWindow(800, 600, "testogl", NULL, NULL);
  if (this->window == NULL){
    _throwError("unable to create a window");
    this->finalize();
    return;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    _throwError("unable to load `glad`");
    return;
  }
  _log("window initialized");
}

void Window::createProgram(std::vector<const char*> shaders) {
  if (!GLFW_INITIALIZED) {
    _throwError("`glfw` not initialized");
    return;
  }
  Program program;
  program.create();
  for (auto shader = shaders.begin(); shader != shaders.end(); ++shader) {
    program.attachShader(*shader);
  } 
  program.link();
  (this->programs).push_back(program.ind);
}

void Window::render(void (*lambda)(GLFWwindow* window, std::vector<unsigned int> programs)) {
  lambda(this->window, this->programs);
}

void Window::finalize() {
  if (!GLFW_INITIALIZED) {
    _throwError("`glfw` not initialized");
    return;
  }
  for (auto p = (this->programs).begin(); p != (this->programs).end(); ++p) {
    glDeleteProgram(*p);
    _log("program " << *p << "  deleted");
  }
}
