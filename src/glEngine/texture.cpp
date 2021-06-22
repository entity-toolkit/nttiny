#include "defs.h"
#include "texture.h"

#include <rapidcsv/rapidcsv.h>
#include <fmt/core.h>
#include <plog/Log.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <vector>

Texture::Texture(int w, int h, float *data) {
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_FLOAT, data);
}

Colormap::Colormap() {
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_1D, id);

  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

Colormap::Colormap(const std::string &filename) : Colormap() {
  readFromCSV(filename);
}

Colormap::Colormap(int n, float *data) : Colormap() { load(n, data); }

void Colormap::load(int n, float *data) {
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_1D, id);
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, n, 0, GL_RGB, GL_FLOAT, data);
  loaded = true;
}

void Colormap::readFromCSV(const std::string &filename) {
  try {
    if (!std::filesystem::exists(filename)) {
      throw -1;
    }
  } catch (int) {
    PLOGE << fmt::format("File {} not found", filename);
    return;
  }
  rapidcsv::Document doc(filename, rapidcsv::LabelParams(-1, -1));
  std::vector<float> reds = doc.GetColumn<float>(0);
  std::vector<float> greens = doc.GetColumn<float>(1);
  std::vector<float> blues = doc.GetColumn<float>(2);
  auto *colormap_colors = new float[reds.size() * 3];
  std::size_t j{0};
  for (std::size_t i{0}; i != reds.size(); ++i) {
    colormap_colors[j + 0] = reds[i];
    colormap_colors[j + 1] = greens[i];
    colormap_colors[j + 2] = blues[i];
    j += 3;
  }
  load(static_cast<int>(reds.size()), colormap_colors);
}
