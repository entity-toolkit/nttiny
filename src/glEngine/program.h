#ifndef GLENGINE_PROGRAM_H
#define GLENGINE_PROGRAM_H

#include "shaders.h"

class Program {
private:
  unsigned int id;
  bool created = false;
  bool linked = false;
public:
  const unsigned int& ind = id;
  const bool& isCreated = created;
  const bool& isLinked = linked;
  Program() = default;
  ~Program() = default;
  void create();
  void attachShader(const char* shader);
  void attachShader(Shader* shader);
  void link();
  void free();
};

#endif
