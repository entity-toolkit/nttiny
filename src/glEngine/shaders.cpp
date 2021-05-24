#include "defs.h"
#include "shaders.h"
#include "aux.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>

Shader::Shader(const char *f_) { fname = f_; }

Shader::~Shader() {
  if (!GLFW_INITIALIZED) {
    _throwError("`glfw` not initialized");
    return;
  }
  if (this->compiled) {
    glDeleteShader(this->id);
  }
  _log("shader deleted");
}

void Shader::loadFromFile() {
  if (this->loaded) {
    _printWarning("shader already loaded -> ignoring the call");
    return;
  }
  // automatically determine the shader type
  std::string type_str = getFileExtension(this->fname);
  if (type_str == ".frag") {
    this->type = GL_FRAGMENT_SHADER;
  } else if (type_str == ".vert") {
    this->type = GL_VERTEX_SHADER;
  } else {
    _throwError("unkown shader");
    return;
  }
  // parse shader from file
  std::string source_str = readFile(this->fname);
  if (source_str.empty()) {
    _throwError("cannot read shader | shader file empty");
    return;
  }
  this->code = source_str;
  this->loaded = true;
  _printDebug("path=" << (this->fname) << " (type=" << (this->type)
                      << ")\n'''\n"
                      << this->code << "'''");
  _log("shader loaded");
}

void Shader::compile() {
  if (this->compiled) {
    _printWarning("shader already compiled -> ignoring call");
    return;
  }
  if (!(this->loaded)) {
    _printWarning("shader not loaded explicitly -> loading implicitly");
    this->loadFromFile();
    if (!(this->loaded))
      return;
  }
  if (!GLFW_INITIALIZED) {
    _throwError("`glfw` not initialized");
  }
  int successQ;
  char infoLog[512];
  this->id = glCreateShader(this->type);
  const GLchar *source = (const GLchar *)(this->code).c_str();
  glShaderSource(this->id, 1, &(source), NULL);
  // compile shader
  glCompileShader(this->id);
  glGetShaderiv(this->id, GL_COMPILE_STATUS, &successQ);
  if (!successQ) {
    glGetShaderInfoLog(this->id, 512, NULL, infoLog);
    _throwError("cannot compile shader\n" << infoLog);
    return;
  }
  this->compiled = true;
  _log("shader compiled");
}
