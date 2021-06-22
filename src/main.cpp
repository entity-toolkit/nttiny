#include "defs.h"

#include "sim.h"

#include "window.h"
#include "shader.h"
#include "sprite.h"
#include "texture.h"
#include "menu.h"

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

  int win_width{800}, win_height{480};

  FakeSimulation m_fakesim(100, 60, 20);
  m_fakesim.setData();

  Window m_window{Window(win_width, win_height, "Nttiny", 0, false)};
  Shader m_shader{Shader("shader.vert", "shader.frag")};

  Texture m_texture{
      Texture(m_fakesim.get_sx(), m_fakesim.get_sy(), m_fakesim.get_data1())};

  Colormap m_colormap;

  Sprite m_sprite{Sprite()};

  Menu m_menu{Menu(m_window.get_window(), &m_fakesim, &m_colormap)};

  m_shader.use();
  m_shader.setInt("field", 0);
  m_shader.setInt("colormap", 1);

  double timer{glfwGetTime()};
  double hard_limit{glfwGetTime()};
  while (!m_window.windowShouldClose()) {
    if (glfwGetTime() >= hard_limit + 1.0 / HARD_LIMIT_FPS) {
      m_window.use(&m_fakesim);

      m_menu.use();
      m_window.setStandardUniforms(m_shader);

      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      m_shader.use();
      m_sprite.draw(&m_fakesim, &m_texture, &m_colormap);
      m_menu.draw();

      m_window.unuse();

      if (glfwGetTime() >=
          timer + 1.0 / static_cast<double>(m_fakesim.get_steps_per_second())) {
        timer = glfwGetTime();
        m_fakesim.updateData();
      }
      hard_limit = glfwGetTime();
    }
  }
  return 0;
}
