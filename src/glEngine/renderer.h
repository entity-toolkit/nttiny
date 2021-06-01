#ifndef GLENGINE_RENDERER_H
#define GLENGINE_RENDERER_H

#include "window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>

class Renderer {
public:
  Window screen;
  virtual void preloop() = 0;
  virtual void loop() = 0;
  virtual void postloop() = 0;
};

#endif
