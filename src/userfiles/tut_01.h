#ifndef TUT_01
#define TUT_01

#include "renderer.h"

#include <imgui.h>

class Gradient : private Renderer {
private:
  unsigned int vbo, vao, ebo;
  ImVec4 clear_color;
public:
  void preloop() override; 
  void loop() override;
  void postloop() override;
};

#endif
