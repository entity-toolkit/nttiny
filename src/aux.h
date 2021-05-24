#ifndef AUX_H
#define AUX_H

#include "defs.h"

#include <plog/Log.h>

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

auto readFile(const char *path) -> std::string {
  std::string code;
  std::ifstream file;
  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    file.open(path);
    std::stringstream stream;
    stream << file.rdbuf();
    file.close();
    code = stream.str();
  } catch (std::ifstream::failure const &e) {
    PLOGE << "cannot read file `" << path << "`";
    return "";
  }
  return code;
}

auto getFileExtension(const char *file_name) -> std::string {
  int ext = '.';
  const char *extension = nullptr;
  extension = strrchr(file_name, ext);
  return static_cast<std::string>(extension);
}

#endif
