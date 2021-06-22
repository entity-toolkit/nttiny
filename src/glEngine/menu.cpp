#include "defs.h"
#include "sim.h"
#include "menu.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>

Menu::Menu(GLFWwindow *window, Simulation *sim) : m_sim(sim) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 150");
  m_fields[0] = "data1";
  m_fields[1] = "data2";
  m_field_selected = 0;
}

Menu::~Menu() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void Menu::use(void (*setup)(Menu *self)) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  (*setup)(this);
  ImGui::Render();
}

void Menu::use() {
  auto default_setup = [](Menu *self) {
    ImGui::Begin("Nttiny");
    // ImGui::ShowDemoWindow();
    ImGui::Text("Timestep: %d", self->m_sim->get_timestep());
    // Choose field component to display
    ImGui::Combo("Field", &(self->m_field_selected), self->m_fields,
                 IM_ARRAYSIZE(self->m_fields));
    // Left step
    ImGui::PushButtonRepeat(true);
    if (ImGui::ArrowButton("##left", ImGuiDir_Left)) {
      self->m_sim->stepBwd();
      if (!self->m_sim->is_paused()) {
        self->m_sim->playToggle();
      }
    }
    ImGui::PopButtonRepeat();
    ImGui::SameLine();
    // Toggle play/pause
    if (ImGui::Button(self->m_sim->is_paused() ? "Play" : "Pause")) {
      self->m_sim->playToggle();
    }
    // Right step
    ImGui::SameLine();
    ImGui::PushButtonRepeat(true);
    if (ImGui::ArrowButton("##right", ImGuiDir_Right)) {
      self->m_sim->stepFwd();
      if (!self->m_sim->is_paused()) {
        self->m_sim->playToggle();
      }
    }
    ImGui::PopButtonRepeat();
    // Simulation speed
    int rate = self->m_sim->get_steps_per_second();
    ImGui::SliderInt("Timesteps per second", &rate, 0, 50);
    self->m_sim->set_steps_per_second(rate);
    // Simulation direction
    int dir = self->m_sim->is_forward() ? 1 : 0;
    const char *directions[2] = {"<<", ">>"};
    const char *direction = directions[dir];
    ImGui::SliderInt("Simulation direction", &dir, 0, 1, direction);
    if (self->m_sim->is_forward() != static_cast<bool>(dir == 1)) {
      self->m_sim->reverse();
    }
    ImGui::End();
  };
  use(default_setup);
}
