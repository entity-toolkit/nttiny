#ifndef GLENGINE_SPRITE_H
#define GLENGINE_SPRITE_H

#include "defs.h"
#include "menu.h"
#include "sim.h"

#include <glm/glm.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Sprite {
public:
  Sprite(GLuint texture_id, GLuint colormap_id);
  ~Sprite();
  void draw(Simulation *sim, Menu *menu);
private:
	GLuint m_vao, m_vbo;
  GLuint m_texture_id;
  GLuint m_colormap_id;
};

#endif
