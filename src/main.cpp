#include "defs.h"

#include "window.h"
#include "shader.h"
#include "sprite.h"

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

  Window m_window{Window(400, 400, "Simulation", 0, false)};
  Shader m_shader{Shader("shader.vert", "shader.frag")};
  Sprite m_sprite{Sprite(0)};

  while (!m_window.windowShouldClose()) {
    m_window.use();

    m_window.setStandardUniforms(m_shader);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_shader.use();
    m_sprite.draw();
    m_window.unuse();
  }
  return 0;
}
