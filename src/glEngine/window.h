#ifndef GLENGINE_WINDOW_H
#define GLENGINE_WINDOW_H

#include "shader.h"
#include "sim.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>

class Window {
public:
	Window(int width, int height, const std::string& name, int swapInterval, bool isFullscreen);
	~Window();
	void use(Simulation *sim);
	void unuse();
	[[nodiscard]] auto windowShouldClose() const -> int { return glfwWindowShouldClose(m_win); }
	[[nodiscard]] auto get_window() const -> GLFWwindow* { return m_win; }
  void setStandardUniforms(const Shader &shader);
private:
	void processInput(Simulation *sim);
	GLFWwindow* m_win;
  float m_xPos, m_yPos;
	int m_winWidth, m_winHeight;
	float m_deltaTime = 0.0f, m_prevFrame = 0.0f;
};

#endif
