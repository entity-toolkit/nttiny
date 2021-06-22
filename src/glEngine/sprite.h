#ifndef GLENGINE_SPRITE_H
#define GLENGINE_SPRITE_H

#include "defs.h"
#include "texture.h"
#include "sim.h"

#include <glm/glm.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Sprite {
public:
  Sprite();
  ~Sprite();
  void draw(Simulation *sim, Texture *texture, Colormap *colormap);
private:
	GLuint m_vao, m_vbo;
};

#endif
