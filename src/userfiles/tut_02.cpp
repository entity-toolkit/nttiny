#include "defs.h"
#include "tut_02.h"
#include "renderer.h"

#include <plog/Log.h>
#include <plog/Init.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/ColorConsoleAppender.h>

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <vector>
#include <cmath>

static void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

void Texture::preloop() {
  this->screen.initialize();
  this->screen.createProgram({"tut_02.vert", "tut_02.frag"});

  glGenTextures(1, &(this->texture));
  glBindTexture(GL_TEXTURE_2D, this->texture);
  // set the texture wrapping/filtering options (on the currently bound texture object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  int width = 100, height = 100;
  float nx, ny;
  auto data = new unsigned char[width * height];
  int cnt = 0;
  for (int j {0}; j < height; ++j) {
    ny = static_cast<float>(j) / height;
    for (int i {0}; i < width; ++i) {
      nx = static_cast<float>(i) / width;
      data[cnt] = 255 * (std::sin(3.0 * M_PI * nx + 14.0 * M_PI * ny) + 1.0) * 0.5;
      ++cnt;
    }
  }
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  // square screen
  float vertices[] = {
     1.0,  1.0, 1.0, 1.0,
     1.0, -1.0, 1.0, 0.0,
    -1.0, -1.0, 0.0, 0.0,
    -1.0,  1.0, 0.0, 1.0
  };
  unsigned int indices[] = {0, 1, 3, 1, 2, 3};

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);
  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)nullptr);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void Texture::loop() {
  glUseProgram(this->screen.programs[0]);
  PLOGV << "render loop started";

  while (!glfwWindowShouldClose(this->screen.window)) {
    processInput(this->screen.window);
    // rendering >

    // setting global uniforms
    auto u_time = static_cast<float>(glfwGetTime());
    glUniform1f(glGetUniformLocation(this->screen.programs[0], "u_time"),
                u_time);
    int width, height;
    glfwGetFramebufferSize(this->screen.window, &width, &height);
    glUniform2f(glGetUniformLocation(this->screen.programs[0], "u_resolution"),
                static_cast<float>(width), static_cast<float>(height));
    double xpos, ypos;
    glfwGetCursorPos(this->screen.window, &xpos, &ypos);
    glUniform2f(glGetUniformLocation(this->screen.programs[0], "u_mouse"),
                static_cast<float>(2.0 * xpos / width),
                static_cast<float>(1.0 - 2.0 * ypos / height));

    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glBindTexture(GL_TEXTURE_2D, this->texture);

    glBindVertexArray(this->vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    // glBindVertexArray(0); // unbind if necessary
    // < rendering

    glfwSwapBuffers(this->screen.window);
    glfwPollEvents();
  }
}

void Texture::postloop() {
  glDeleteVertexArrays(1, &(this->vao));
  glDeleteBuffers(1, &(this->vbo));
  glDeleteBuffers(1, &(this->ebo));
}
