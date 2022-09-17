#ifndef STYLE_H
#define STYLE_H

#include "cousine.h"
#include "jetbrains.h"
#include "fa-regular.h"
#include "fa-solid.h"
#include "icons.h"

#include <plog/Log.h>
#include <imgui.h>

#include <stdexcept>

void SetupStyle() {
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_None;

  int hue = 150;
  float col_main_sat = 180.f / 255.f;
  float col_main_val = 161.f / 255.f;
  float col_area_sat = 124.f / 255.f;
  float col_area_val = 100.f / 255.f;
  float col_back_sat = 59.f / 255.f;
  float col_back_val = 40.f / 255.f;

  ImGuiStyle& style = ImGui::GetStyle();

  ImVec4 col_text = ImColor::HSV(hue / 255.f, 20.f / 255.f, 235.f / 255.f);
  ImVec4 col_main = ImColor::HSV(hue / 255.f, col_main_sat, col_main_val);
  ImVec4 col_back = ImColor::HSV(hue / 255.f, col_back_sat, col_back_val);
  ImVec4 col_area = ImColor::HSV(hue / 255.f, col_area_sat, col_area_val);

  style.Colors[ImGuiCol_Text] = ImVec4(col_text.x, col_text.y, col_text.z, 1.00f);
  style.Colors[ImGuiCol_TextDisabled] = ImVec4(col_text.x, col_text.y, col_text.z, 0.58f);
  style.Colors[ImGuiCol_WindowBg] = ImVec4(col_back.x, col_back.y, col_back.z, 1.00f);
  style.Colors[ImGuiCol_Border] = ImVec4(col_text.x, col_text.y, col_text.z, 0.30f);
  style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  style.Colors[ImGuiCol_FrameBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
  style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.68f);
  style.Colors[ImGuiCol_FrameBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
  style.Colors[ImGuiCol_TitleBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.45f);
  style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(col_main.x, col_main.y, col_main.z, 0.35f);
  style.Colors[ImGuiCol_TitleBgActive] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
  style.Colors[ImGuiCol_MenuBarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 0.57f);
  style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(col_area.x, col_area.y, col_area.z, 1.00f);
  style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.31f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
  style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
  style.Colors[ImGuiCol_CheckMark] = ImVec4(col_main.x, col_main.y, col_main.z, 0.80f);
  style.Colors[ImGuiCol_SliderGrab] = ImVec4(col_main.x, col_main.y, col_main.z, 0.24f);
  style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
  style.Colors[ImGuiCol_Button] = ImVec4(col_main.x, col_main.y, col_main.z, 0.44f);
  style.Colors[ImGuiCol_ButtonHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
  style.Colors[ImGuiCol_ButtonActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
  style.Colors[ImGuiCol_Header] = ImVec4(col_main.x, col_main.y, col_main.z, 0.76f);
  style.Colors[ImGuiCol_HeaderHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.86f);
  style.Colors[ImGuiCol_HeaderActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
  style.Colors[ImGuiCol_ResizeGrip] = ImVec4(col_main.x, col_main.y, col_main.z, 0.20f);
  style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 0.78f);
  style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
  style.Colors[ImGuiCol_PlotLines] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
  style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
  style.Colors[ImGuiCol_PlotHistogram] = ImVec4(col_text.x, col_text.y, col_text.z, 0.63f);
  style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(col_main.x, col_main.y, col_main.z, 1.00f);
  style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(col_main.x, col_main.y, col_main.z, 0.43f);

  try {
    ImFontConfig icons_config;
    ImGui::GetIO().Fonts->Clear();
    ImGui::GetIO().Fonts->AddFontDefault();
    // ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
    //     JetBrains_compressed_data, JetBrains_compressed_size, 14.0f);
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_16_FA, 0};
    ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
        faRegular_compressed_data, faRegular_compressed_size, 8.0f, &icons_config, icons_ranges);
    ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
        faSolid_compressed_data, faSolid_compressed_size, 8.0f, &icons_config, icons_ranges);
    ImGui::GetIO().FontGlobalScale = 1.0f;
    ImGui::GetIO().Fonts->Build();
  }
  catch (std::runtime_error& e) {
    PLOGW << "Warning: " << e.what();
  }

  style.WindowRounding = 0.0f;
  style.ChildRounding = 0.0f;
  style.FrameRounding = 0.0f;
  style.GrabRounding = 0.0f;
  style.PopupRounding = 0.0f;
  style.ScrollbarRounding = 0.0f;
  style.TabRounding = 0.0f;

  style.AntiAliasedLinesUseTex = true;
  style.AntiAliasedLines = true;
  style.AntiAliasedFill = true;
}

#endif