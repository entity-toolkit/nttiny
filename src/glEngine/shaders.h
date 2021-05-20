#ifndef GLENGINE_SHADERS_H
#define GLENGINE_SHADERS_H

#include "defs.h"

#include <string>

class Shader {
private:
  unsigned int id;
  unsigned int type;
  bool compiled = false;
  bool loaded = false;
  const char* fname;
  std::string code;
public:
  const unsigned int& ind = id;
  Shader(const char* f_);
  ~Shader();
  void loadFromFile();
  void compile();
  const bool& isCompiled = compiled;
  const bool& isLoaded = loaded;
};

#endif
