#ifndef GLENGINE_WINDOW_H
#define GLENGINE_WINDOW_H

#include "renderer.h"
#include "shaders.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>

class Window {
public:
  GLFWwindow* window;
  std::vector<unsigned int> programs;
public:
  Window() = default; 
  void initialize();
  void createProgram(const std::vector<const char *>& shaders);
  //void createProgram(std::vector<const char*> shaders);
  //void render(Renderer &rnd);
  void finalize();
  ~Window() = default;
  friend class Renderer;
};

#endif
