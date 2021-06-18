#ifndef GLENGINE_SPRITE_H
#define GLENGINE_SPRITE_H

#include "defs.h"

#include <glm/glm.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Sprite {
public:
  Sprite(GLuint texture_id);
  ~Sprite();
  void draw();
private:
	GLuint m_vao, m_vbo, m_ebo;
  GLuint m_texture_id;
};

#endif
