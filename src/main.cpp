#include "defs.h"
#include "program.h"
#include "shaders.h"
#include "window.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <vector>

bool GLFW_INITIALIZED{false};

static void processInput(GLFWwindow *window);

auto main() -> int {
  if (!glfwInit()) {
    _throwError("unable to initialize `glfw`");
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
    nttiny.createProgram({"shader.vert", "green.frag"});
    nttiny.createProgram({"shader.vert", "red.frag"});
    auto myloop = [](GLFWwindow *window, std::vector<unsigned int> programs) {
      if (!GLFW_INITIALIZED) {
        _throwError("`glfw` not initialized");
        return;
      }
      float first_triangle[] = {-0.9f, -0.5f, -0.0f, -0.5f, -0.45f, 0.5f};
      float second_triangle[] = {0.0f, -0.5f, 0.9f, -0.5f, 0.45f, 0.5f};
      unsigned int vbo_s[2], vao_s[2];
      glGenBuffers(2, vbo_s);
      glGenVertexArrays(2, vao_s);

      glBindVertexArray(vao_s[0]);
      glBindBuffer(GL_ARRAY_BUFFER, vbo_s[0]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(first_triangle), first_triangle,
                   GL_STATIC_DRAW);
      glVertexAttribPointer(
          0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
          (const void *)nullptr); // Vertex attributes stay the same
      glEnableVertexAttribArray(0);

      glBindVertexArray(vao_s[1]); // note that we bind to a different VAO now
      glBindBuffer(GL_ARRAY_BUFFER, vbo_s[1]); // and a different VBO
      glBufferData(GL_ARRAY_BUFFER, sizeof(second_triangle), second_triangle,
                   GL_STATIC_DRAW);
      glVertexAttribPointer(
          0, 2, GL_FLOAT, GL_FALSE, 0,
          (const void
               *)nullptr); // because the vertex data is tightly packed we can
                           // also specify 0 as the vertex attribute's stride
                           // to let OpenGL figure it out
      glEnableVertexAttribArray(0);

      // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

      _log("render loop started");

      while (!glfwWindowShouldClose(window)) {
        processInput(window);
        // rendering >
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(programs[0]);
        glBindVertexArray(vao_s[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glUseProgram(programs[1]);
        glBindVertexArray(vao_s[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(0); // unbind if necessary
        // < rendering

        glfwSwapBuffers(window);
        glfwPollEvents();
      }
    };
    nttiny.render(myloop);
    nttiny.finalize();
  }

  if (!GLFW_INITIALIZED) {
    _throwError("`glfw` not initialized or terminated prematurely");
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
