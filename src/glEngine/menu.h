#ifndef GLENGINE_MENU_H
#define GLENGINE_MENU_H

#include "sim.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>

class Menu {
public:
  Menu(GLFWwindow *window, Simulation *sim);
  ~Menu();
  void use();
  void use(void (*setup)(Menu *self));
  void draw() { ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); };
  auto field_selected() const -> int { return m_field_selected; }
private:
  const char* m_fields[2];
  int m_field_selected;
  Simulation* m_sim;
};

#endif
