#include "defs.h"
#include "program.h"
#include "shaders.h"
#include "window.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <vector>

bool GLFW_INITIALIZED{false};

static void processInput(GLFWwindow *window);

int main() {
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
      float firstTriangle[] = {-0.9f, -0.5f, -0.0f, -0.5f, -0.45f, 0.5f};
      float secondTriangle[] = {0.0f, -0.5f, 0.9f, -0.5f, 0.45f, 0.5f};
      unsigned int VBOs[2], VAOs[2];
      glGenBuffers(2, VBOs);
      glGenVertexArrays(2, VAOs);

      glBindVertexArray(VAOs[0]);
      glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(firstTriangle), firstTriangle,
                   GL_STATIC_DRAW);
      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                            (const void *)0); // Vertex attributes stay the same
      glEnableVertexAttribArray(0);

      glBindVertexArray(VAOs[1]); // note that we bind to a different VAO now
      glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]); // and a different VBO
      glBufferData(GL_ARRAY_BUFFER, sizeof(secondTriangle), secondTriangle,
                   GL_STATIC_DRAW);
      glVertexAttribPointer(
          0, 2, GL_FLOAT, GL_FALSE, 0,
          (const void *)0); // because the vertex data is tightly packed we can
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
        glBindVertexArray(VAOs[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glUseProgram(programs[1]);
        glBindVertexArray(VAOs[1]);
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
// float vertices[] = {
// 0.5f,  0.5f,
// 0.5f, -0.5f,
//-0.5f, -0.5f,
//-0.5f,  0.5f
//};
// unsigned int indices[] = {
// 0, 1, 3,
// 1, 2, 3
//};
// unsigned int VAO, VBO, EBO;
// glGenBuffers(1, &VBO);
// glGenBuffers(1, &EBO);
// glGenVertexArrays(1, &VAO);
// glBindVertexArray(VAO);

// glBindBuffer(GL_ARRAY_BUFFER, VBO);
// glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
// glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
// GL_STATIC_DRAW);

// glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const
// void*)0); glEnableVertexAttribArray(0); glBindBuffer(GL_ARRAY_BUFFER, 0);

//// unbind VAO
// glBindVertexArray(0);
////glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

//_log("render loop started");

// while (!glfwWindowShouldClose(this->window)) {
// processInput(this->window);

//// rendering >
// glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
// glClear(GL_COLOR_BUFFER_BIT);

// glUseProgram((this->programs).back().ind);
// glBindVertexArray(VAO);
// glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
// glBindVertexArray(0); // unbind if necessary
//// < rendering

// glfwSwapBuffers(this->window);
// glfwPollEvents();
//}
