#include "defs.h"
#include "shader.h"

#include <fmt/core.h>
#include <plog/Log.h>
#include <glm/glm.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <sstream>

Shader::Shader(const GLchar *vertexPath, const GLchar *fragmentPath,
               const GLchar *geometryPath) {
  // Read file
  std::string vertex_code, fragment_code;
  std::ifstream vsh_file, fsh_file;

  vsh_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fsh_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    vsh_file.open(vertexPath);
    fsh_file.open(fragmentPath);
    std::stringstream vsh_stream, fsh_stream;
    vsh_stream << vsh_file.rdbuf();
    fsh_stream << fsh_file.rdbuf();
    vsh_file.close();
    fsh_file.close();
    vertex_code = vsh_stream.str();
    fragment_code = fsh_stream.str();
  } catch (std::ifstream::failure e) {
    PLOGE << fmt::format("Cannot read shader: {}, {}.", vertexPath, fragmentPath);
  }
  const char *vsh_code = vertex_code.c_str();
  const char *fsh_code = fragment_code.c_str();

  // Compile shaders
  unsigned int vertex, fragment;

  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vsh_code, nullptr);
  glCompileShader(vertex);
  checkCompileErrors(vertex, "VERTEX");

  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fsh_code, nullptr);
  glCompileShader(fragment);
  checkCompileErrors(fragment, "FRAGMENT");

  id = glCreateProgram();
  glAttachShader(id, vertex);
  glAttachShader(id, fragment);

  glDeleteShader(vertex);
  glDeleteShader(fragment);

  if (geometryPath != nullptr) {
    std::string geometry_code;
    std::ifstream gsh_file;

    gsh_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
      gsh_file.open(geometryPath);
      std::stringstream gsh_stream;
      gsh_stream << gsh_file.rdbuf();
      gsh_file.close();
      geometry_code = gsh_stream.str();
    } catch (std::ifstream::failure e) {
      PLOGE << "Cannot read geometry shader.";
    }
    const char *gsh_code = geometry_code.c_str();

    unsigned int geometry = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometry, 1, &gsh_code, nullptr);
    glCompileShader(geometry);
    checkCompileErrors(geometry, "GEOMETRY");
    glAttachShader(id, geometry);
    glDeleteShader(geometry);
  }
  glLinkProgram(id);
  checkCompileErrors(id, "PROGRAM");
}

void Shader::use() { glUseProgram(id); }

void Shader::setBg() {
  float vertices[] = {
      -1.0, -1.0, 1.0, -1.0, 1.0, 1.0, -1.0, 1.0,
  };
  unsigned int indices[] = {0, 1, 2, 2, 3, 0};

  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_vbo);
  glGenBuffers(1, &m_ebo);
  glBindVertexArray(m_vao);

  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                        (void *)nullptr);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void Shader::useBg() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glBindVertexArray(m_vao);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void Shader::checkCompileErrors(GLuint shader, const std::string &type) {
  GLint success;
  GLchar info_log[1024];
  if (type != "PROGRAM") {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shader, 1024, nullptr, info_log);
      PLOGE << fmt::format("Cannot compile {} shader.\nError log:\n{}", type, info_log);
    }
  } else {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shader, 1024, nullptr, info_log);
      PLOGE << "Program linking error: \n" << info_log;
      PLOGE << fmt::format("Program linking error.\nError log:\n{}", info_log);
    }
  }
}
