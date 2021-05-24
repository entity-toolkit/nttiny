#include "defs.h"
#include "shaders.h"
#include "aux.h"

#include <plog/Log.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>

Shader::Shader(const char *f_) { fname = f_; }

Shader::~Shader() {
  if (!GLFW_INITIALIZED) {
    PLOGE << "`glfw` not initialized";
    return;
  }
  if (this->compiled) {
    glDeleteShader(this->id);
  }
  PLOGV << "shader deleted";
}

void Shader::loadFromFile() {
  if (this->loaded) {
    PLOGW << "shader already loaded -> ignoring the call";
    return;
  }
  // automatically determine the shader type
  std::string type_str = getFileExtension(this->fname);
  if (type_str == ".frag") {
    this->type = GL_FRAGMENT_SHADER;
  } else if (type_str == ".vert") {
    this->type = GL_VERTEX_SHADER;
  } else {
    PLOGE << "unkown shader";
    return;
  }
  // parse shader from file
  std::string source_str = readFile(this->fname);
  if (source_str.empty()) {
    PLOGE << "cannot read shader | shader file empty";
    return;
  }
  this->code = source_str;
  this->loaded = true;
  PLOGD << "path=" << (this->fname) << " (type=" << (this->type) << ")\n'''\n"
        << this->code << "'''";
  PLOGV << "shader loaded";
}

void Shader::compile() {
  if (this->compiled) {
    PLOGW << "shader already compiled -> ignoring call";
    return;
  }
  if (!(this->loaded)) {
    PLOGW << "shader not loaded explicitly -> loading implicitly";
    this->loadFromFile();
    if (!(this->loaded))
      return;
  }
  if (!GLFW_INITIALIZED) {
    PLOGE << "`glfw` not initialized";
  }
  int success;
  char info_log[512];
  this->id = glCreateShader(this->type);
  const auto *source = static_cast<const GLchar *>((this->code).c_str());
  glShaderSource(this->id, 1, &(source), nullptr);
  // compile shader
  glCompileShader(this->id);
  glGetShaderiv(this->id, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(this->id, 512, nullptr, info_log);
    PLOGE << "cannot compile shader\n" << info_log;
    return;
  }
  this->compiled = true;
  PLOGV << "shader compiled";
}
