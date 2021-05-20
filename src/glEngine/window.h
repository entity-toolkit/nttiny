#ifndef GLENGINE_WINDOW_H
#define GLENGINE_WINDOW_H

#include "shaders.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>

class Window {
private:
  GLFWwindow* window;
  std::vector<unsigned int> programs;
public:
  Window() = default; 
  void initialize();
  void createProgram(std::vector<const char*> shaders);
  void render(void (*lambda)(GLFWwindow* window, std::vector<unsigned int> programs));
  void finalize();
  ~Window() = default;
};

#endif
