#include "defs.h"

#include "sim.h"

#include "window.h"
#include "shader.h"
#include "sprite.h"
#include "texture.h"

#include <fmt/core.h>

#include <plog/Log.h>
#include <plog/Init.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/ColorConsoleAppender.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

auto main() -> int {
  static plog::ColorConsoleAppender<plog::TxtFormatter> console_appender;
  plog::Severity max_severity;
#ifdef VERBOSE
  max_severity = plog::verbose;
#elif DEBUG
  max_severity = plog::debug;
#else
  max_severity = plog::warning;
#endif
  plog::init(max_severity, &console_appender);

  int win_width = 400, win_height = 400;

  Simulation m_fakesim(20, 40);
  m_fakesim.setData();

  Window m_window{Window(win_width, win_height, "Simulation", 0, false)};
  Shader m_shader{Shader("shader.vert", "shader.frag")};
  Texture m_texture{
      Texture(m_fakesim.get_sx(), m_fakesim.get_sy(), m_fakesim.get_data1())};
  Sprite m_sprite{Sprite(m_texture.id)};

  while (!m_window.windowShouldClose()) {
    m_window.use();

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_fakesim.get_sx(),
                    m_fakesim.get_sy(), GL_RED, GL_FLOAT,
                    m_fakesim.get_data1());

    m_window.setStandardUniforms(m_shader);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_shader.use();
    m_sprite.draw();
    m_window.unuse();

    m_fakesim.updateData();
  }
  return 0;
}
