#include "defs.h"
#include "vis.h"

#include "api.h"
#include "style.h"

#include <plog/Log.h>
#include <plog/Init.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/ColorConsoleAppender.h>

#include <toml.hpp>

#include <implot.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <utility>
#include <memory>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <stdexcept>

namespace nttiny {

template <class T, ushort D>
Visualization<T, D>::Visualization(float scale, int win_width, int win_height, bool resizable)
    : m_win_width(win_width), m_win_height(win_height), m_resizable(resizable) {
  plog::Severity max_severity;
#ifdef VERBOSE
  max_severity = plog::verbose;
#elif defined(DEBUG)
  max_severity = plog::debug;
#else
  max_severity = plog::warning;
#endif
  plog::init<VISPLOGID>(max_severity, &m_console_appender);

  glfwInit();
  m_window = std::make_unique<Window>(m_win_width * (int)(scale / 2.0f),
                                      m_win_height * (int)(scale / 2.0f),
                                      "Nttiny",
                                      1,
                                      resizable);
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();

  SetupStyle(scale);

  ImGui_ImplGlfw_InitForOpenGL(m_window->get_window(), true);
  ImGui_ImplOpenGL3_Init(m_window->get_glsl_version());
}

template <class T, ushort D>
Visualization<T, D>::~Visualization() {
  PLOGD_(VISPLOGID) << "Destroying Visualization.";
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImPlot::DestroyContext();
  ImGui::DestroyContext();
  glfwDestroyWindow(m_window->get_window());
  glfwTerminate();
}

template <class T, ushort D>
void Visualization<T, D>::addPcolor2d(float vmin, float vmax) {
  PLOGD_(VISPLOGID) << "Opening Pcolor2d.";
  auto myplot{std::make_unique<Pcolor2d<T>>(this->m_id, vmin, vmax)};
  ++this->m_id;
  this->m_plots.push_back(std::move(myplot));
  this->bindSimulation();
}

template <class T, ushort D>
void Visualization<T, D>::addScatter2d() {
  PLOGD_(VISPLOGID) << "Opening Scatter2d.";
  auto myplot{std::make_unique<Scatter2d<T>>(this->m_id)};
  ++this->m_id;
  this->m_plots.push_back(std::move(myplot));
  this->bindSimulation();
}

template <class T, ushort D>
void Visualization<T, D>::bindSimulation() {
  for (auto plot{this->m_plots.begin()}; plot != this->m_plots.end(); ++plot) {
    (*plot)->bindSimulation(this->m_sim);
  }
}

template <class T, ushort D>
void Visualization<T, D>::bindSimulation(SimulationAPI<T, D>* sim) {
  PLOGD_(VISPLOGID) << "Binding simulation.";
  this->m_sim = sim;
  for (auto plot{this->m_plots.begin()}; plot != this->m_plots.end(); ++plot) {
    (*plot)->bindSimulation(this->m_sim);
  }
}

template <class T, ushort D>
void Visualization<T, D>::drawControls() {
  ImGui::BeginChild("simulation control");
  ImGui::TextWrapped("t = %.3f [%d dt]", this->m_sim->get_time(), this->m_sim->get_timestep());
  {
    /* ------------------------------ step backward ----------------------------- */
    ImGui::PushButtonRepeat(true);
    if (ImGui::Button(ICON_FA_BACKWARD_STEP,
                      ImVec2(1 * ImGui::GetFontSize(), 2 * ImGui::GetFontSize()))) {
      this->m_sim->stepBwd();
      this->m_sim->m_data_changed = true;
      if (!this->m_sim->is_paused()) { m_sim->playToggle(); }
    }
    ImGui::PopButtonRepeat();

    /* ------------------------------- play/pause ------------------------------- */
    ImGui::SameLine();
    if (ImGui::Button(this->m_sim->is_paused() ? ICON_FA_PLAY : ICON_FA_PAUSE,
                      ImVec2(2 * ImGui::GetFontSize(), 2 * ImGui::GetFontSize()))) {
      this->m_sim->playToggle();
    }

    /* ------------------------------ step forward ------------------------------ */
    ImGui::SameLine();
    ImGui::PushButtonRepeat(true);
    if (ImGui::Button(ICON_FA_FORWARD_STEP,
                      ImVec2(1 * ImGui::GetFontSize(), 2 * ImGui::GetFontSize()))) {
      this->m_sim->stepFwd();
      this->m_sim->m_data_changed = true;
      if (!this->m_sim->is_paused()) { this->m_sim->playToggle(); }
    }
    ImGui::PopButtonRepeat();

    /* --------------------------------- restart -------------------------------- */
    ImGui::SameLine(ImGui::GetWindowWidth() - 2 * ImGui::GetFontSize());
    if (ImGui::Button(ICON_FA_ARROW_ROTATE_LEFT,
                      ImVec2(2 * ImGui::GetFontSize(), 2 * ImGui::GetFontSize()))) {
      if (!this->m_sim->is_paused()) { m_sim->playToggle(); }
      this->m_sim->restart();
    }

    /* -------------------------------- direction ------------------------------- */
    int dir = this->m_sim->is_forward() ? 1 : 0;
    const char* directions[2] = {ICON_FA_BACKWARD, ICON_FA_FORWARD};
    const char* direction = directions[dir];
    ImGui::SetNextItemWidth(4 * ImGui::GetFontSize());
    ImGui::SliderInt("##direction", &dir, 0, 1, direction);
    if (this->m_sim->is_forward() != static_cast<bool>(dir == 1)) { this->m_sim->reverse(); }

    /* ----------------------------- skip timesteps ----------------------------- */
    ImGui::SameLine();
    int jmp{this->m_sim->get_jumpover()};
    ImGui::DragInt("##skip", &jmp, 1, 1, 1000, "t += %d dt");
    this->m_sim->set_jumpover(jmp);
  }

  ImGui::EndChild();
}

template <class T, ushort D>
void Visualization<T, D>::processControllerInput() {
  static bool pressing_spacebar = false;
  if (glfwGetKey(this->m_window->get_window(), GLFW_KEY_SPACE) == GLFW_PRESS) {
    pressing_spacebar = true;
  }
  if (glfwGetKey(this->m_window->get_window(), GLFW_KEY_SPACE) == GLFW_RELEASE
      && (pressing_spacebar)) {
    pressing_spacebar = false;
    this->m_sim->playToggle();
  }

  if (glfwGetKey(this->m_window->get_window(), GLFW_KEY_LEFT) == GLFW_PRESS) {
    this->m_sim->stepBwd();
    this->m_sim->m_data_changed = true;
  }
  if (glfwGetKey(this->m_window->get_window(), GLFW_KEY_RIGHT) == GLFW_PRESS) {
    this->m_sim->stepFwd();
    this->m_sim->m_data_changed = true;
  }

  static bool pressing_left = false;
  if (glfwGetKey(m_window->get_window(), GLFW_KEY_COMMA) == GLFW_PRESS) { pressing_left = true; }
  if (glfwGetKey(this->m_window->get_window(), GLFW_KEY_COMMA) == GLFW_RELEASE && (pressing_left)) {
    pressing_left = false;
    this->m_sim->stepBwd();
    this->m_sim->m_data_changed = true;
  }
  static bool pressing_right = false;
  if (glfwGetKey(this->m_window->get_window(), GLFW_KEY_PERIOD) == GLFW_PRESS) {
    pressing_right = true;
  }
  if (glfwGetKey(this->m_window->get_window(), GLFW_KEY_PERIOD) == GLFW_RELEASE
      && (pressing_right)) {
    pressing_right = false;
    this->m_sim->stepFwd();
    this->m_sim->m_data_changed = true;
  }
}

template <class T, ushort D>
void Visualization<T, D>::drawMainMenuBar() {
  ImGui::BeginMainMenuBar();

  if (ImGui::BeginMenu("Add plot")) {
    if (ImGui::MenuItem("Pcolor")) { addPcolor2d(0, 1); }
    if (ImGui::MenuItem("Scatter")) { addScatter2d(); }
    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("State")) {
    if (ImGui::MenuItem(ICON_FA_FLOPPY_DISK " save")) {
      auto rewrite{true};
      auto cntr{0};
      for (auto plot{this->m_plots.begin()}; plot != this->m_plots.end(); ++plot) {
        ++cntr;
        auto metadata = (*plot)->exportMetadata();
        metadata.writeToFile(STATE_FILENAME, rewrite);
        rewrite = false;
      }
      std::ofstream export_file;
      export_file.open(STATE_FILENAME, std::fstream::app);
      if (export_file.is_open()) {
        export_file << "[Plot]\nnpanels = " << cntr << "\n";
        export_file.close();
      }
    }
    if (ImGui::MenuItem(ICON_FA_UPLOAD " load")) {
      this->m_plots.clear();
      try {
        auto input = toml::parse("nttiny.toml");
        const auto& panels = toml::find(input, "Plot");
        auto npanels = toml::find<int>(panels, "npanels");
        for (int i{0}; i < npanels; ++i) {
          const auto& plot = toml::find(panels, std::to_string(i));
          PlotMetadata metadata;
          metadata.m_field_selected = toml::find<int>(plot, "field_selected");
          metadata.m_type = toml::find<std::string>(plot, "type");
          metadata.m_cmap = toml::find<std::string>(plot, "cmap");
          metadata.m_vmax = toml::find<float>(plot, "vmax");
          metadata.m_vmin = toml::find<float>(plot, "vmin");
          metadata.m_log = toml::find<bool>(plot, "log");
          if (metadata.m_type == "Pcolor2d") {
            addPcolor2d(metadata.m_vmin, metadata.m_vmax);
            this->m_plots.back()->importMetadata(metadata);
          } else if (metadata.m_type == "Scatter2d") {
            addScatter2d();
            this->m_plots.back()->importMetadata(metadata);
          }
        }
      }
      catch (std::exception& err) {
        PLOGE_(VISPLOGID) << "Error loading state: " << err.what();
      }
    }
    ImGui::EndMenu();
  }
  static bool config_window{false};

  // if (ImGui::BeginMenu("Configure ui")) {
  //   // ImGui::OpenPopup("Configure ui");
  //   config_window = true;
  //   ImGui::EndMenu();
  // }
  // ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  // ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
  // ImGui::Begin("Configure ui", &config_window);
  // if (ImGui::BeginPopupContextItem()) // <-- This is using IsItemHovered()
  // {
  //   if (ImGui::MenuItem("Close")) { config_window = false; }
  //   ImGui::Text("outline color");
  //   ImGui::SameLine();
  //   float outline_color[4]{UI_Settings.OutlineColor.x,
  //                          UI_Settings.OutlineColor.y,
  //                          UI_Settings.OutlineColor.z,
  //                          UI_Settings.OutlineColor.w};
  //   ImGui::ColorEdit4("OutlineColorPicker##",
  //                     outline_color,
  //                     ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
  //   UI_Settings.OutlineColor
  //       = ImVec4(outline_color[0], outline_color[1], outline_color[2], outline_color[3]);
  //   ImGui::EndPopup();
  // }
  // ImGui::End();

  // static bool test_window = false;
  // ImGui::Checkbox("Hovered/Active tests after Begin() for title bar testing", &test_window);
  // if (test_window) {
  //   ImGui::Begin("Title bar Hovered/Active tests", &test_window);
  //   if (ImGui::BeginPopupContextItem()) // <-- This is using IsItemHovered()
  //   {
  //     if (ImGui::MenuItem("Close")) { test_window = false; }
  //     ImGui::EndPopup();
  //   }
  //   ImGui::Text("IsItemHovered() after begin = %d (== is title bar hovered)\n"
  //               "IsItemActive() after begin = %d (== is window being clicked/moved)\n",
  //               ImGui::IsItemHovered(),
  //               ImGui::IsItemActive());
  //   ImGui::End();
  // }
  ImGui::EndMainMenuBar();
}

template <class T, ushort D>
void Visualization<T, D>::loop() {
  PLOGD_(VISPLOGID) << "Starting Visualization loop.";
  int jumpover_counter{-1};

  // float prev_scale = 0.f;
  // float xscale, yscale;
  // glfwGetWindowContentScale(this->m_window->get_window(), &xscale, &yscale);
  // if (xscale != prev_scale) {
  //   prev_scale = xscale;
  //   ImGui::GetIO().Fonts->Clear();
  //   ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
  //       JetBrains_compressed_data, JetBrains_compressed_size, xscale * 16.0f);
  //   ImGui::GetIO().Fonts->Build();
  //   ImGui_ImplOpenGL3_DestroyFontsTexture();
  //   ImGui_ImplOpenGL3_CreateFontsTexture();
  // }

  auto& Sim = this->m_sim;
  auto& Grid = this->m_sim->m_global_grid;
  const auto coord = Grid.m_coord;
  const auto ngh = Grid.m_ngh;
  const auto sx1 = Grid.m_size[0];
  const auto sx2 = Grid.m_size[1];
  auto dx1 = Grid.m_xi[0][1] - Grid.m_xi[0][0];
  auto x1min = Grid.m_xi[0][0] - ngh * dx1;
  auto x1max = Grid.m_xi[0][sx1] + ngh * dx1;
  auto dx2 = Grid.m_xi[1][1] - Grid.m_xi[1][0];
  auto x2min = Grid.m_xi[1][0] - ngh * dx2;
  auto x2max = Grid.m_xi[1][sx2] + ngh * dx2;

  if (coord == Coord::Spherical) {
    this->SharedAxes.X.Min = 0.0f;
    this->SharedAxes.X.Max = (float)x1max;
    this->SharedAxes.Y.Min = -(float)x1max;
    this->SharedAxes.Y.Max = (float)x1max;
  } else {
    this->SharedAxes.X.Min = (float)x1min;
    this->SharedAxes.X.Max = (float)x1max;
    this->SharedAxes.Y.Min = (float)x2min;
    this->SharedAxes.Y.Max = (float)x2max;
  }

  while (!this->m_window->windowShouldClose()) {
    this->m_window->processInput();
    this->processControllerInput();
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ++jumpover_counter;
    Sim->updateData(jumpover_counter < 0 || (jumpover_counter % (Sim->get_jumpover()) == 0));

    this->drawMainMenuBar();

    std::vector<bool> close_plots;

    static bool use_work_area = true;
    static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove
                                  | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(use_work_area ? viewport->WorkPos : viewport->Pos);
    ImGui::SetNextWindowSize(use_work_area ? viewport->WorkSize : viewport->Size);
    bool open = true;

    if (ImGui::Begin("main dashboard", &open, flags)) {
      {
        ImGui::BeginChild("controls", ImVec2(15 * ImGui::GetFontSize(), 0), true);
        this->drawControls();
        ImGui::EndChild();
      }
      ImGui::SameLine();
      {
        ImGui::BeginChild("plots", ImVec2(-1, -1), false);
        auto rows = std::fmax((int)std::ceil(this->m_plots.size() / 3.0f), 1);
        auto cols = std::fmin(std::fmax(this->m_plots.size(), 1), 3);
        if (ImPlot::BeginSubplots(
                "##subplots", rows, cols, ImVec2(-1, -1), ImPlotSubplotFlags_None)) {
          for (std::size_t i{0}; i < this->m_plots.size(); ++i) {
            ImGui::PushID(i);
            close_plots.push_back(this->m_plots[i]->draw(this->SharedAxes, this->UI_Settings));
            ImGui::PopID();
          }
          ImPlot::EndSubplots();
        }
        ImGui::EndChild();
      }
    }

    for (std::size_t i{0}; i < close_plots.size(); ++i) {
      if (close_plots[i]) {
        this->m_plots.erase(this->m_plots.begin() + i);
        close_plots.erase(close_plots.begin() + i);
        --i;
      }
    }

    ImGui::End();
    ImGui::Render();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    this->m_window->unuse();
    this->m_sim->m_data_changed = false;
  }
}
} // namespace nttiny

template class nttiny::Visualization<float, 2>;
template class nttiny::Visualization<double, 2>;
