#include "defs.h"
#include "sim.h"
#include "menu.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <algorithm>

#include <iostream>
#include <filesystem>

Menu::Menu(GLFWwindow *window, Simulation *sim, Colormap *cmap)
    : m_sim(sim), m_cmap(cmap) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 150");
  // read colormaps
  for (const auto &entry : std::filesystem::directory_iterator(".")) {
    std::string cmap = entry.path();
    int i0{static_cast<int>(cmap.find(".cmap.csv"))};
    if (i0 >= 0) {
      m_colormaps.push_back(cmap.erase(i0, 9).erase(0, 2));
    }
  }
  m_active_colormap = 0;
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
    ImGui::Begin("Menu");
    // ImGui::ShowDemoWindow();
    ImGui::Text("Timestep: %d", self->m_sim->get_timestep());
    // Left step
    {
      ImGui::PushButtonRepeat(true);
      if (ImGui::ArrowButton("##left", ImGuiDir_Left)) {
        self->m_sim->stepBwd();
        if (!self->m_sim->is_paused()) {
          self->m_sim->playToggle();
        }
      }
      ImGui::PopButtonRepeat();
    }
    // Toggle play/pause
    {
      ImGui::SameLine();
      if (ImGui::Button(self->m_sim->is_paused() ? "Play" : "Pause")) {
        self->m_sim->playToggle();
      }
    }

    // Right step
    {
      ImGui::SameLine();
      ImGui::PushButtonRepeat(true);
      if (ImGui::ArrowButton("##right", ImGuiDir_Right)) {
        self->m_sim->stepFwd();
        if (!self->m_sim->is_paused()) {
          self->m_sim->playToggle();
        }
      }
      ImGui::PopButtonRepeat();
    }

    // Choose field component to display
    {
      ImGui::Text("Field component to plot:");
      int field_selected{self->m_sim->field_selected};
      ImGui::Combo("", &field_selected, self->m_sim->fields,
                   IM_ARRAYSIZE(self->m_sim->fields));
      if (self->m_sim->field_selected != field_selected) {
        self->m_sim->field_selected = field_selected;
        self->m_sim->updated = true;
      }
    }
    // Simulation speed
    {
      int rate = self->m_sim->get_steps_per_second();
      ImGui::Text("Simulation rate:");
      ImGui::SetNextItemWidth(std::max(ImGui::GetContentRegionAvail().x * 0.5f,
                                       ImGui::GetFontSize() * 6));
      ImGui::SliderInt("dt per second", &rate, 0, HARD_LIMIT_FPS);
      self->m_sim->set_steps_per_second(rate);
    }
    // Simulation direction
    {
      int dir = self->m_sim->is_forward() ? 1 : 0;
      const char *directions[2] = {"<<", ">>"};
      const char *direction = directions[dir];
      ImGui::Text("Simulation direction:");
      ImGui::SetNextItemWidth(std::max(ImGui::GetContentRegionAvail().x * 0.2f,
                                       ImGui::GetFontSize() * 4));
      ImGui::SliderInt(self->m_sim->is_forward() ? "Forward" : "Backward", &dir,
                       0, 1, direction);
      if (self->m_sim->is_forward() != static_cast<bool>(dir == 1)) {
        self->m_sim->reverse();
      }
    }
    // Select colormap
    {
      std::size_t cmap_selected = self->m_active_colormap;
      ImGui::Text("Colormap:");
      ImGui::SameLine();
      if (ImGui::BeginCombo(self->m_colormaps[cmap_selected].c_str(),
                            self->m_colormaps[cmap_selected].c_str(),
                            ImGuiComboFlags_NoPreview)) {
        for (std::size_t n{0}; n != self->m_colormaps.size(); ++n) {
          const bool is_selected = (cmap_selected == n);
          if (ImGui::Selectable(self->m_colormaps[n].c_str(), is_selected)) {
            cmap_selected = n;
          }
          if (is_selected) {
            ImGui::SetItemDefaultFocus();
          }
        }
        ImGui::EndCombo();
      }
      if ((cmap_selected != self->m_active_colormap) ||
          (!self->m_cmap->loaded)) {
        self->m_active_colormap = cmap_selected;
        self->m_cmap->readFromCSV(self->m_colormaps[cmap_selected] +
                                  ".cmap.csv");
      }
    }
    ImGui::End();
  };
  use(default_setup);
}
