#ifndef TUT_02
#define TUT_02

#include "renderer.h"

#include <imgui.h>

class Texture : private Renderer {
private:
  unsigned int vbo, vao, ebo;
  ImVec4 clear_color;
public:
  void preloop() override; 
  void loop() override;
  void postloop() override;
};

#endif
