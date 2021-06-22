#include "defs.h"

#include "sim.h"

#include "window.h"
#include "shader.h"
#include "sprite.h"
#include "texture.h"
#include "menu.h"

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

  int win_width = 800, win_height = 800;

  Simulation m_fakesim(100, 60, 20);
  m_fakesim.setData();

  Window m_window{Window(win_width, win_height, "Simulation", 0, false)};
  Shader m_shader{Shader("shader.vert", "shader.frag")};

  Texture m_texture{
      Texture(m_fakesim.get_sx(), m_fakesim.get_sy(), m_fakesim.get_data1())};

  float colormap_colors[12];

  colormap_colors[0] = 0.0f;
  colormap_colors[1] = 0.0f;
  colormap_colors[2] = 0.0f;

  colormap_colors[3] = 0.0f;
  colormap_colors[4] = 0.0f;
  colormap_colors[5] = 1.0f;

  colormap_colors[6] = 1.0f;
  colormap_colors[7] = 0.0f;
  colormap_colors[8] = 0.0f;

  colormap_colors[9] = 1.0f;
  colormap_colors[10] = 1.0f;
  colormap_colors[11] = 1.0f;
  Colormap m_colormap{Colormap(4, colormap_colors)};

  Sprite m_sprite{Sprite(m_texture.id, m_colormap.id)};

  Menu m_menu{Menu(m_window.get_window(), &m_fakesim)};

  m_shader.use();
  m_shader.setInt("field", 0);
  m_shader.setInt("colormap", 1);

  double timer = glfwGetTime();
  while (!m_window.windowShouldClose()) {
    m_window.use();

    m_menu.use();
    m_window.setStandardUniforms(m_shader);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_shader.use();
    m_sprite.draw(&m_fakesim, &m_menu);
    m_menu.draw();

    m_window.unuse();

    if (glfwGetTime() >=
        timer + 1.0 / static_cast<double>(m_fakesim.get_steps_per_second())) {
      timer += 1.0 / static_cast<double>(m_fakesim.get_steps_per_second());
      m_fakesim.updateData();
    }
  }
  return 0;
}
