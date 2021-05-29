#include "defs.h"
#include "program.h"
#include "shaders.h"
#include "window.h"

#include <fmt/core.h>

#include <plog/Log.h>
#include <plog/Init.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/ColorConsoleAppender.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <iostream>
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
  //fmt::print("Hello, world!\n");
  //return 0;

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

      IMGUI_CHECKVERSION();
      ImGui::CreateContext();
      ImGuiIO &io = ImGui::GetIO();
      (void)io;
      ImGui::StyleColorsDark();
      // ImGui::StyleColorsClassic();

      ImGui_ImplGlfw_InitForOpenGL(window, true);
      ImGui_ImplOpenGL3_Init("#version 150");
      bool show_demo_window = true;
      ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

      float vertices[] = {
          -1.0, -1.0, 1.0, -1.0, 1.0, 1.0, -1.0, 1.0,
      };
      unsigned int indices[] = {0, 1, 2, 2, 3, 0};

      unsigned int vbo, vao, ebo;
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

      glUseProgram(programs[0]);
      PLOGV << "render loop started";

      while (!glfwWindowShouldClose(window)) {
        processInput(window);
        // rendering >

        // imgui >
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        if (show_demo_window)
          ImGui::ShowDemoWindow(&show_demo_window);

        {
          static float f = 0.0f;
          static int counter = 0;

          ImGui::Begin("Hello, world!");
          ImGui::Text("This is some useful text.");
          ImGui::Checkbox("Demo Window", &show_demo_window);

          ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
          ImGui::ColorEdit3("clear color", (float *)&clear_color);

          if (ImGui::Button("Button"))
            counter++;
          ImGui::SameLine();
          ImGui::Text("counter = %d", counter);
          ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                      1000.0f / ImGui::GetIO().Framerate,
                      ImGui::GetIO().Framerate);
          ImGui::End();
        }
        ImGui::Render();
        // < imgui

        // setting global uniforms
        float u_time = glfwGetTime();
        glUniform1f(glGetUniformLocation(programs[0], "u_time"), u_time);
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glUniform2f(glGetUniformLocation(programs[0], "u_resolution"),
                    static_cast<float>(width), static_cast<float>(height));
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        glUniform2f(glGetUniformLocation(programs[0], "u_mouse"),
                    static_cast<float>(2.0 * xpos / width),
                    static_cast<float>(1.0 - 2.0 * ypos / height));

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        // glBindVertexArray(0); // unbind if necessary
        // < rendering

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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
// float vertices[] = {
//// positions        // colors
// 0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
//-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
// 0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f  // top
//};
// unsigned int vbo, vao;
// glGenBuffers(1, &vbo);
// glGenVertexArrays(1, &vao);

// glBindVertexArray(vao);
// glBindBuffer(GL_ARRAY_BUFFER, vbo);
// glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
// glVertexAttribPointer(
// 0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
//(const void *)nullptr); // Vertex attributes stay the same
// glEnableVertexAttribArray(0);
// glVertexAttribPointer(
// 1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
//(const void *)(3 * sizeof(float))); // Vertex attributes stay the same
// glEnableVertexAttribArray(1);
