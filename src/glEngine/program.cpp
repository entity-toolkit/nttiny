#include "defs.h"
#include "program.h"
#include "shaders.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void Program::create() {
  if (!GLFW_INITIALIZED) {
    _throwError("`glfw` not initialized");
    return;
  }
  this->id = glCreateProgram(); 
  this->created = true;
  _log("program " << this->id << " created");
}

void Program::attachShader(const char* shader) {
  this->attachShader(new Shader(shader));
}
void Program::attachShader(Shader* shader) {
  if (!GLFW_INITIALIZED) {
    _throwError("`glfw` not initialized");
    return;
  }
  if (!shader->isCompiled) {
    _printWarning("shader not explicitly compiled -> compiling implicitly");
    shader->compile();
    if (!shader->isCompiled) return;
  }
  glAttachShader(this->id, shader->ind);
  _log("shader attached");
}

void Program::link() {
  if (!GLFW_INITIALIZED) {
    _throwError("`glfw` not initialized");
    return;
  }
  if (!(this->created)) {
    _printWarning("shader not explicitly created -> creating implicitly");
    this->create();
  }
  glLinkProgram(this->id);
  int successQ;
  char infoLog[512];
  glGetProgramiv(this->id, GL_LINK_STATUS, &successQ);
  if(!successQ) {
    glGetProgramInfoLog(this->id, 512, NULL, infoLog);
    _throwError("cannot link program\n" << infoLog);
    return;
  }
  this->linked = true;
  _log("program " << this->id << " linked");
}
