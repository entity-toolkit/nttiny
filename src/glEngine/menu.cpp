#include "defs.h"

#include "menu.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

Menu::Menu(GLFWwindow *window) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 150");
  //  bool show_demo_window = true;
  // ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
}

Menu::~Menu() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void Menu::use() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  //  if (show_demo_window)
  //   ImGui::ShowDemoWindow(&show_demo_window);
  {
    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("Hello, world!");
    ImGui::Text("This is some useful text.");
    // ImGui::Checkbox("Demo Window", &show_demo_window);

    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
    // ImGui::ColorEdit3("clear color", (float *)&clear_color);

    if (ImGui::Button("Button"))
      counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
  }
  ImGui::Render();
}

void Menu::draw() { ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); }
