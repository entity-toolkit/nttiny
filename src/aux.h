#ifndef AUX_H
#define AUX_H

#include "defs.h"

#include <iostream>
#include <string_view>
#include <string>
#include <cstring>
#include <sstream>
#include <fstream>

std::string readFile(const char *path) {
  std::string code;
  std::ifstream file;
  file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
  try {
    file.open(path);
    std::stringstream stream;
    stream << file.rdbuf();
    file.close();
    code = stream.str();
  } catch(std::ifstream::failure const &e) {
    _throwError("cannot read file `" << path << "`");
    return ""; 
  }
  return code;
}

std::string getFileExtension(const char* file_name) {
  int ext = '.';
  const char* extension = NULL;
  extension = strrchr(file_name, ext);
  return static_cast<std::string>(extension);
}

#endif
