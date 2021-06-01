#include "defs.h"
#include "tut_01.h"
#include "renderer.h"

#include <plog/Log.h>
#include <plog/Init.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/ColorConsoleAppender.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <vector>

static void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

void Gradient::preloop() {
  this->screen.initialize();
  this->screen.createProgram({"shader.vert", "shader.frag"});

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(this->screen.window, true);
  ImGui_ImplOpenGL3_Init("#version 150");
  this->clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  float vertices[] = {
      -1.0, -1.0, 1.0, -1.0, 1.0, 1.0, -1.0, 1.0,
  };
  unsigned int indices[] = {0, 1, 2, 2, 3, 0};

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);
  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                        (void *)nullptr);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}
void Gradient::loop() {
  glUseProgram(this->screen.programs[0]);
  PLOGV << "render loop started";

  while (!glfwWindowShouldClose(this->screen.window)) {
    processInput(this->screen.window);
    // rendering >

    // imgui >
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    // if (show_demo_window)
    // ImGui::ShowDemoWindow(&show_demo_window);

    {
      static float f = 0.0f;
      static int counter = 0;

      ImGui::Begin("Hello, world!");
      ImGui::Text("This is some useful text.");
      // ImGui::Checkbox("Demo Window", &show_demo_window);

      ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
      ImGui::ColorEdit3("clear color", (float *)&(this->clear_color));

      if (ImGui::Button("Button"))
        counter++;
      ImGui::SameLine();
      ImGui::Text("counter = %d", counter);
      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                  1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
      ImGui::End();
    }
    ImGui::Render();
    // < imgui

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

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(this->vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    // glBindVertexArray(0); // unbind if necessary
    // < rendering

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(this->screen.window);
    glfwPollEvents();
  }
}
void Gradient::postloop() {
  glDeleteVertexArrays(1, &(this->vao));
  glDeleteBuffers(1, &(this->vbo));
  glDeleteBuffers(1, &(this->ebo));
}
