#include "vis.h"

#include "api.h"
#include "defs.h"
#include "export.h"
#include "imgui_notify.h"
#include "plots.h"
#include "style.h"
#include "tools.h"

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/imgui.h>
#include <implot/implot.h>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Init.h>
#include <plog/Log.h>
#include <toml11/toml.hpp>

#include <filesystem>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

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
    m_window = std::make_unique<Window>((int)(m_win_width * scale / 2.0f),
                                        (int)(m_win_height * scale / 2.0f),
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
  void Visualization<T, D>::addPcolor2d() {
    PLOGD_(VISPLOGID) << "Opening Pcolor2d.";
    auto myplot { std::make_unique<Pcolor2d<T>>(this->m_id) };
    ++this->m_id;
    this->m_plots.push_back(std::move(myplot));
    this->bindSimulation();
  }

  template <class T, ushort D>
  void Visualization<T, D>::addScatter2d() {
    PLOGD_(VISPLOGID) << "Opening Scatter2d.";
    auto myplot { std::make_unique<Scatter2d<T>>(this->m_id) };
    ++this->m_id;
    this->m_plots.push_back(std::move(myplot));
    this->bindSimulation();
  }

  template <class T, ushort D>
  void Visualization<T, D>::addTimePlot() {
    PLOGD_(VISPLOGID) << "Opening TimePlot.";
    auto myplot { std::make_unique<TimePlot<T, D>>(this->m_id) };
    ++this->m_id;
    this->m_plots.push_back(std::move(myplot));
    this->bindSimulation();
  }

  template <class T, ushort D>
  void Visualization<T, D>::bindSimulation() {
    for (auto plot { this->m_plots.begin() }; plot != this->m_plots.end(); ++plot) {
      (*plot)->bindSimulation(this->m_sim);
    }
  }

  template <class T, ushort D>
  void Visualization<T, D>::bindSimulation(SimulationAPI<T, D>* sim) {
    PLOGD_(VISPLOGID) << "Binding simulation.";
    this->m_sim = sim;
    for (auto plot { this->m_plots.begin() }; plot != this->m_plots.end(); ++plot) {
      (*plot)->bindSimulation(this->m_sim);
    }
  }

  template <class T, ushort D>
  void Visualization<T, D>::drawControls() {
    ImGui::BeginChild("controls",
                      this->m_collapsed_controls ? ImVec2(2 * ImGui::GetFontSize(), 0)
                                                 : ImVec2(15 * ImGui::GetFontSize(), 0),
                      true);
    if (!this->m_collapsed_controls) {
      ImGui::SameLine(ImGui::GetWindowWidth() - 4 * ImGui::GetFontSize()
                      - 2 * ImGui::GetStyle().FramePadding.x);
      ImGui::TextDisabled("(?)");
      if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted("Click to collapse the control sidebar.");
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
      }
      ImGui::SameLine(ImGui::GetWindowWidth() - 2 * ImGui::GetFontSize()
                      - 2 * ImGui::GetStyle().FramePadding.x);
    }
    auto size = this->m_collapsed_controls
                  ? ImVec2(-1, 2 * ImGui::GetFontSize())
                  : ImVec2(2 * ImGui::GetFontSize(), 2 * ImGui::GetFontSize());
    if (ImGui::Button(this->m_collapsed_controls ? ICON_FA_RIGHT_LONG : ICON_FA_LEFT_LONG,
                      size)) {
      this->m_collapsed_controls = !this->m_collapsed_controls;
    }
    ImGui::Separator();
    if (this->m_collapsed_controls) {
      ImGui::EndChild();
      return;
    }

    ImGui::TextWrapped(
      "t = %.3f [%d dt]", this->m_sim->get_time(), this->m_sim->get_timestep());
    {
      /* ------------------------------ step backward ----------------------------- */
      ImGui::PushButtonRepeat(true);
      if (ImGui::Button(ICON_FA_BACKWARD_STEP,
                        ImVec2(1 * ImGui::GetFontSize(), 2 * ImGui::GetFontSize()))) {
        this->m_sim->stepBwd();
        this->m_sim->m_data_changed = true;
        if (!this->m_sim->is_paused()) {
          m_sim->playToggle();
        }
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
        if (!this->m_sim->is_paused()) {
          this->m_sim->playToggle();
        }
      }
      ImGui::PopButtonRepeat();

      /* --------------------------------- restart -------------------------------- */
      ImGui::SameLine(ImGui::GetWindowWidth() - 2 * ImGui::GetFontSize());
      if (ImGui::Button(ICON_FA_ARROW_ROTATE_LEFT,
                        ImVec2(2 * ImGui::GetFontSize(), 2 * ImGui::GetFontSize()))) {
        if (!this->m_sim->is_paused()) {
          m_sim->playToggle();
        }
        this->m_sim->restart();
      }

      /* -------------------------------- direction ------------------------------- */
      int         dir           = this->m_sim->is_forward() ? 1 : 0;
      const char* directions[2] = { ICON_FA_BACKWARD, ICON_FA_FORWARD };
      const char* direction     = directions[dir];
      ImGui::SetNextItemWidth(2 * ImGui::GetFontSize());
      ImGui::SliderInt("##direction", &dir, 0, 1, direction);
      if (this->m_sim->is_forward() != static_cast<bool>(dir == 1)) {
        this->m_sim->reverse();
      }

      /* ----------------------------- skip timesteps ----------------------------- */
      ImGui::SameLine();
      int jmp { this->m_sim->get_jumpover() };
      ImGui::DragInt("##skip", &jmp, 1, 1, 1000, "t += %d dt");
      this->m_sim->set_jumpover(jmp);
    }
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::PushID("snapshot");
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(6.0f / 7.0f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          (ImVec4)ImColor::HSV(6.0f / 7.0f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(6.0f / 7.0f, 0.8f, 0.8f));
    if (ImGui::Button(ICON_FA_IMAGE " save snapshot", ImVec2(-1, 2 * ImGui::GetFontSize()))) {
      this->m_save_image = true;
    }
    ImGui::PopStyleColor(3);
    ImGui::PopID();

    ImGui::PushID("record");
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.575f, 1.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.575f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.575f, 0.8f, 0.8f));
    if (ImGui::Button(this->m_save_video ? ICON_FA_VIDEO_SLASH " stop recording"
                                         : ICON_FA_VIDEO " start recording",
                      ImVec2(-1, 2 * ImGui::GetFontSize()))) {
      this->m_save_video = !this->m_save_video;
      if (!this->m_save_video) {
        ImGui::InsertNotification(
          { ImGuiToastType_Info, 3000, "Frames for video saved in `%s`", "bin/frames/" });
        m_save_videoframe_count = 0;
      }
    }
    ImGui::PopStyleColor(3);
    ImGui::PopID();

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
    if (glfwGetKey(m_window->get_window(), GLFW_KEY_COMMA) == GLFW_PRESS) {
      pressing_left = true;
    }
    if (glfwGetKey(this->m_window->get_window(), GLFW_KEY_COMMA) == GLFW_RELEASE
        && (pressing_left)) {
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

    if (ImGui::BeginMenu("Menu")) {
      ImGui::MenuItem("(plots)", NULL, false, false);
      if (ImGui::MenuItem("add pcolor")) {
        addPcolor2d();
      }
      if (ImGui::MenuItem("add scatter")) {
        addScatter2d();
      }
      if (ImGui::MenuItem("add timeplot")) {
        addTimePlot();
      }
      ImGui::Separator();
      ImGui::MenuItem("(state)", NULL, false, false);
      if (ImGui::MenuItem(ICON_FA_FLOPPY_DISK " save")) {
        saveState();
      }
      if (ImGui::MenuItem(ICON_FA_UPLOAD " load")) {
        loadState();
      }
      ImGui::Separator();
      if (ImGui::BeginMenu("configure ui")) {
        ImGui::Text("domain outline");
        ImGui::SameLine();
        float outline_color[4] { UI_Settings.OutlineColor.x,
                                 UI_Settings.OutlineColor.y,
                                 UI_Settings.OutlineColor.z,
                                 UI_Settings.OutlineColor.w };
        ImGui::ColorEdit4(
          "##", outline_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
        UI_Settings.OutlineColor
          = ImVec4(outline_color[0], outline_color[1], outline_color[2], outline_color[3]);
        ImGui::EndMenu();
      }
      ImGui::EndMenu();
    }
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
      ImGui::BeginTooltip();
      ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
      ImGui::TextUnformatted("Right click the plot legends to open plot controller.");
      ImGui::PopTextWrapPos();
      ImGui::EndTooltip();
    }
    ImGui::EndMainMenuBar();
  }

  template <class T, ushort D>
  void Visualization<T, D>::loadState() {
    this->m_plots.clear();
    try {
      auto        input         = toml::parse(STATE_FILENAME);
      const auto& panels        = toml::find(input, "Plot");
      auto        npanels       = toml::find_or<int>(panels, "npanels", 0);
      auto        outline_color = toml::find_or<std::vector<float>>(
        panels, "outline_color", { 1.0f, 1.0f, 1.0f, 1.0f });
      this->UI_Settings.OutlineColor
        = ImVec4(outline_color[0], outline_color[1], outline_color[2], outline_color[3]);
      auto range = toml::find_or<std::vector<float>>(panels, "range", { 0, 1, 0, 1 });
      this->SharedAxes.X.Min     = range[0];
      this->SharedAxes.X.Max     = range[1];
      this->SharedAxes.Y.Min     = range[2];
      this->SharedAxes.Y.Max     = range[3];
      this->m_collapsed_controls = toml::find_or<bool>(panels, "collapsed_controls", false);
      for (int i { 0 }; i < npanels; ++i) {
        const auto& plot = toml::find(panels, std::to_string(i));
        const auto  type = toml::find<std::string>(plot, "type");
        if (type == "Pcolor2d") {
          addPcolor2d();
        } else if (type == "Scatter2d") {
          addScatter2d();
        } else if (type == "TimePlot") {
          addTimePlot();
        }
        this->m_plots.back()->importMetadata(plot);
      }
    } catch (std::exception& err) {
      PLOGE_(VISPLOGID) << "Error loading state: " << err.what();
    }
    ImGui::InsertNotification(
      { ImGuiToastType_Success, 3000, "Loaded the state from `%s`", (STATE_FILENAME) });
  }
  template <class T, ushort D>
  void Visualization<T, D>::saveState() {
    auto rewrite { true };
    auto cntr { 0 };
    for (auto plot { this->m_plots.begin() }; plot != this->m_plots.end(); ++plot) {
      ++cntr;
      auto* metadata = (*plot)->exportMetadata();
      metadata->writeToFile(STATE_FILENAME, rewrite);
      rewrite = false;
    }
    std::ofstream export_file;
    export_file.open(STATE_FILENAME, std::fstream::app);
    if (export_file.is_open()) {
      export_file << "[Plot]\nnpanels = " << cntr << "\n";
      export_file
        << std::fixed << std::setprecision(3) << "outline_color = ["
        << this->UI_Settings.OutlineColor.x << ", " << this->UI_Settings.OutlineColor.y << ", "
        << this->UI_Settings.OutlineColor.z << ", " << this->UI_Settings.OutlineColor.w
        << "]\n";
      export_file << std::fixed << std::setprecision(3) << "range = ["
                  << this->SharedAxes.X.Min << ", " << this->SharedAxes.X.Max << ", "
                  << this->SharedAxes.Y.Min << ", " << this->SharedAxes.Y.Max << "]\n";
      export_file << "collapsed_controls = " << (this->m_collapsed_controls ? "true" : "false")
                  << "\n";
      export_file.close();
    }
    ImGui::InsertNotification(
      { ImGuiToastType_Success, 3000, "Saved state as `%s`", (STATE_FILENAME) });
  }

  template <class T, ushort D>
  void Visualization<T, D>::loop() {
    PLOGD_(VISPLOGID) << "Starting Visualization loop.";
    int        jumpover_counter { -1 };

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

    auto&      Sim   = this->m_sim;
    auto&      Grid  = this->m_sim->m_global_grid;
    const auto coord = Grid.m_coord;
    const auto ngh   = Grid.m_ngh;
    const auto sx1   = Grid.m_size[0];
    const auto sx2   = Grid.m_size[1];
    auto       dx1   = Grid.m_xi[0][1] - Grid.m_xi[0][0];
    auto       x1min = Grid.m_xi[0][0] - ngh * dx1;
    auto       x1max = Grid.m_xi[0][sx1] + ngh * dx1;
    auto       dx2   = Grid.m_xi[1][1] - Grid.m_xi[1][0];
    auto       x2min = Grid.m_xi[1][0] - ngh * dx2;
    auto       x2max = Grid.m_xi[1][sx2] + ngh * dx2;

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

    try {
      toml::parse(STATE_FILENAME);
      loadState();
    } catch (std::exception& err) {
      PLOGI_(VISPLOGID) << "No state file found\n";
    }

    while (!this->m_window->windowShouldClose()) {
      this->m_window->processInput();
      this->processControllerInput();
      glfwPollEvents();

      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      ++jumpover_counter;
      const bool jumpover_set
        = (jumpover_counter < 0 || (jumpover_counter % (Sim->get_jumpover()) == 0));
      Sim->updateData(jumpover_set);

      this->drawMainMenuBar();

      std::vector<bool>       close_plots;

      static bool             use_work_area = true;
      static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove
                                      | ImGuiWindowFlags_NoResize
                                      | ImGuiWindowFlags_NoSavedSettings;
      const ImGuiViewport* viewport = ImGui::GetMainViewport();
      ImGui::SetNextWindowPos(use_work_area ? viewport->WorkPos : viewport->Pos);
      ImGui::SetNextWindowSize(use_work_area ? viewport->WorkSize : viewport->Size);
      bool open = true;

      if (ImGui::Begin("main dashboard", &open, flags)) {
        { this->drawControls(); }
        ImGui::SameLine();
        {
          ImGui::BeginChild("plots", ImVec2(-1, -1), false);
          auto rows = std::fmax((int)std::ceil(this->m_plots.size() / 3.0f), 1);
          auto cols = std::fmin(std::fmax(this->m_plots.size(), 1), 3);
          if (ImPlot::BeginSubplots(
                "##subplots", rows, cols, ImVec2(-1, -1), ImPlotSubplotFlags_NoAlign)) {
            for (std::size_t i { 0 }; i < this->m_plots.size(); ++i) {
              ImGui::PushID(this->m_plots[i]->getId());
              close_plots.push_back(
                this->m_plots[i]->draw(this->SharedAxes, this->UI_Settings));
              ImGui::PopID();
            }
            ImPlot::EndSubplots();
          }
          ImGui::EndChild();
        }
      }

      for (std::size_t i { 0 }; i < close_plots.size(); ++i) {
        if (close_plots[i]) {
          this->m_plots.erase(this->m_plots.begin() + i);
          close_plots.erase(close_plots.begin() + i);
          --i;
        }
      }

      ImGui::End();
      ImGui::RenderNotifications();
      ImGui::Render();

      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      this->m_window->unuse();
      if (this->m_save_image) {
        const auto fname = this->m_sim->get_title() + "_"
                           + std::to_string(this->m_sim->get_timestep()) + ".png";
        const auto fdir = this->m_sim->get_title() + "/";
        saveImage(fname, fdir);
        this->m_save_image = false;
        ImGui::InsertNotification(
          { ImGuiToastType_Info, 3000, "Snapshot saved as `%s`", (fdir + fname).c_str() });
      }

      if ((this->m_save_video) && (this->m_sim->m_data_changed) && (jumpover_set)) {
        saveImage(
          "frame_" + tools::zeroPadLeft(std::to_string(m_save_videoframe_count), 5) + ".png",
          this->m_sim->get_title() + "/frames/");
        ++m_save_videoframe_count;
      }

      this->m_sim->m_data_changed = false;
    }
  }
}    // namespace nttiny

template class nttiny::Visualization<float, 2>;
template class nttiny::Visualization<double, 2>;
