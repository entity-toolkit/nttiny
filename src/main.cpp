#include "defs.h"
#include "tut_02.h"

#include <fmt/core.h>

#include <plog/Log.h>
#include <plog/Init.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/ColorConsoleAppender.h>

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <iostream>
#include <vector>

bool GLFW_INITIALIZED{false};

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
    Texture myprogram;
    myprogram.preloop();
    myprogram.loop();
    myprogram.postloop();
  }

  if (!GLFW_INITIALIZED) {
    PLOGE << "`glfw` not initialized or terminated prematurely";
  }
  glfwTerminate();
  GLFW_INITIALIZED = false;
  return 0;
}
