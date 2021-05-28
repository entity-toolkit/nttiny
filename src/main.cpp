#include "defs.h"
#include "program.h"
#include "shaders.h"
#include "window.h"

#include <plog/Log.h>
#include <plog/Init.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/ColorConsoleAppender.h>

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <vector>

bool GLFW_INITIALIZED{false};

static void processInput(GLFWwindow *window);

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

  if (!glfwInit()) {
    PLOGE << "unable to initialize `glfw`";
    return -1;
  } else {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFW_INITIALIZED = true;
  }

  {
    Window nttiny;
    nttiny.initialize();
    nttiny.createProgram({"shader.vert", "shader.frag"}); // program #0
    auto myloop = [](GLFWwindow *window, std::vector<unsigned int> programs) {
      if (!GLFW_INITIALIZED) {
        PLOGE << "`glfw` not initialized";
        return;
      }
      float vertices[] = {
          // positions         // colors
          0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
          -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
          0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f  // top
      };
      unsigned int vbo, vao;
      glGenBuffers(1, &vbo);
      glGenVertexArrays(1, &vao);

      glBindVertexArray(vao);
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
      glVertexAttribPointer(
          0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
          (const void *)nullptr); // Vertex attributes stay the same
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(
          1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
          (const void *)(3 * sizeof(float))); // Vertex attributes stay the same
      glEnableVertexAttribArray(1);

      // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

      glUseProgram(programs[0]);
      PLOGV << "render loop started";

      while (!glfwWindowShouldClose(window)) {
        processInput(window);
        // rendering >
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // glBindVertexArray(0); // unbind if necessary
        // < rendering

        glfwSwapBuffers(window);
        glfwPollEvents();
      }
    };
    nttiny.render(myloop);
    nttiny.finalize();
  }

  if (!GLFW_INITIALIZED) {
    PLOGE << "`glfw` not initialized or terminated prematurely";
  }
  glfwTerminate();
  GLFW_INITIALIZED = false;
  return 0;
}

static void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}
