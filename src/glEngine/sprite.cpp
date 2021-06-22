#include "defs.h"
#include "sprite.h"
#include "texture.h"
#include "sim.h"

#include <glm/glm.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

Sprite::Sprite() {
  glGenBuffers(1, &m_vbo);
  glGenVertexArrays(1, &m_vao);

  glBindVertexArray(m_vao);
  glm::vec4 vertices[6];

  vertices[0] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
  vertices[1] = glm::vec4(1.0f, -1.0f, 1.0f, 0.0f);
  vertices[2] = glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f);
  vertices[3] = glm::vec4(1.0f, -1.0f, 1.0f, 0.0f);
  vertices[4] = glm::vec4(-1.0f, -1.0f, 0.0f, 0.0f);
  vertices[5] = glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f);

  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        reinterpret_cast<void *>(0));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        reinterpret_cast<void *>(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(m_vao);
}

Sprite::~Sprite() {
  glDeleteVertexArrays(1, &m_vao);
  glDeleteBuffers(1, &m_vbo);
}

void Sprite::draw(Simulation *sim, Texture *texture, Colormap *colormap) {
  if (sim->updated) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->id);
    switch (sim->field_selected) {
    case 0:
      glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, sim->get_sx(), sim->get_sy(),
                      GL_RED, GL_FLOAT, sim->get_data1());
      break;
    case 1:
      glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, sim->get_sx(), sim->get_sy(),
                      GL_RED, GL_FLOAT, sim->get_data2());
      break;
    default:
      break;
    }
    sim->updated = false;
  }
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_1D, colormap->id);

  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
