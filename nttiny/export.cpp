#include "export.h"

#include <implot/implot.h>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

#include <stb/stb_image_write.h>

#pragma GCC diagnostic pop

#undef STB_IMAGE_WRITE_IMPLEMENTATION

#if defined(__cplusplus) && __cplusplus >= 201703L && defined(__has_include)
#  if __has_include(<filesystem>)
#    define GHC_USE_STD_FS
#    include <filesystem>
namespace fs = std::filesystem;
#  endif
#endif
#ifndef GHC_USE_STD_FS
#  include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;
#endif

void saveImage(const std::string& fileName, const std::string& path) {
  ImGuiIO io = ImGui::GetIO();
  int x = 0;
  int y = 0;
  int nWidth = io.DisplaySize.x * io.DisplayFramebufferScale.x;
  int nHeight = io.DisplaySize.y * io.DisplayFramebufferScale.y;

  std::vector<unsigned char> pixels(4 * nWidth * nHeight);
  glReadPixels(x, y, nWidth, nHeight, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

  stbi_flip_vertically_on_write(true);

  if (!fs::exists(path)) { fs::create_directory(path); }
  stbi_write_png((path + fileName).c_str(), nWidth, nHeight, 4, pixels.data(), 4 * nWidth);
}