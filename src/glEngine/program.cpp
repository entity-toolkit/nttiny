#include "defs.h"
#include "program.h"
#include "shaders.h"

#include <plog/Log.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <array>

void Program::create() {
  if (!GLFW_INITIALIZED) {
    PLOGE << "`glfw` not initialized";
    return;
  }
  this->id = glCreateProgram();
  this->created = true;
  PLOGV << "program " << this->id << " created";
}

void Program::attachShader(const char *shader) {
  Shader sh(shader);
  this->attachShader(&sh);
}
void Program::attachShader(Shader *shader) {
  if (!GLFW_INITIALIZED) {
    PLOGE << "`glfw` not initialized";
    return;
  }
  if (!shader->isCompiled) {
    PLOGW << "shader not explicitly compiled -> compiling implicitly";
    shader->compile();
    if (!shader->isCompiled)
      return;
  }
  glAttachShader(this->id, shader->ind);
  PLOGV << "shader attached";
}

void Program::link() {
  if (!GLFW_INITIALIZED) {
    PLOGE << "`glfw` not initialized";
    return;
  }
  if (!(this->created)) {
    PLOGW << "shader not explicitly created -> creating implicitly";
    this->create();
  }
  glLinkProgram(this->id);
  int success;
  char info_log[512];
  glGetProgramiv(this->id, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(this->id, 512, nullptr, info_log);
    PLOGE << "cannot link program\n" << info_log;
    return;
  }
  this->linked = true;
  PLOGV << "program " << this->id << " linked";
}
