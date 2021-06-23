#ifndef GLENGINE_WINDOW_H
#define GLENGINE_WINDOW_H

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
private:
	void processInput(Simulation *sim);
	GLFWwindow* m_win;
	int m_winWidth, m_winHeight;
};

#endif
