#ifndef GLENGINE_MENU_H
#define GLENGINE_MENU_H

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Menu {
public:
  Menu(GLFWwindow *window);
  ~Menu();
  void draw();
  void use();
private:

};

#endif
