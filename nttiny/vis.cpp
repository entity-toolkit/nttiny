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
Visualization<T, D>::Visualization(int win_width, int win_height, bool resizable)
    : m_win_width(win_width), m_win_height(win_height), m_resizable(resizable) {
  plog::Severity max_severity;
#ifdef VERBOSE
  max_severity = plog::verbose;
#elif DEBUG
  max_severity = plog::debug;
#else
  max_severity = plog::warning;
#endif
  plog::init<VISPLOGID>(max_severity, &m_console_appender);

  glfwInit();
  m_window = std::make_unique<Window>(m_win_width, m_win_height, "Nttiny", 0, resizable);
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();

  SetupStyle();

  ImGui_ImplGlfw_InitForOpenGL(m_window->get_window(), true);
  ImGui_ImplOpenGL3_Init("#version 150");
}

template <class T, ushort D>
Visualization<T, D>::~Visualization() {
  PLOGD_(VISPLOGID) << "Destroying Visualization.";
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImPlot::DestroyContext();
  ImGui::DestroyContext();
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
    if (ImGui::Button(ICON_FA_BACKWARD_STEP, ImVec2(20, 25))) {
      this->m_sim->stepBwd();
      if (!this->m_sim->is_paused()) { m_sim->playToggle(); }
    }
    ImGui::PopButtonRepeat();

    /* ------------------------------- play/pause ------------------------------- */
    ImGui::SameLine();
    if (ImGui::Button(this->m_sim->is_paused() ? ICON_FA_PLAY : ICON_FA_PAUSE, ImVec2(25, 25))) {
      this->m_sim->playToggle();
    }

    /* ------------------------------ step forward ------------------------------ */
    ImGui::SameLine();
    ImGui::PushButtonRepeat(true);
    if (ImGui::Button(ICON_FA_FORWARD_STEP, ImVec2(20, 25))) {
      this->m_sim->stepFwd();
      if (!this->m_sim->is_paused()) { this->m_sim->playToggle(); }
    }
    ImGui::PopButtonRepeat();

    /* --------------------------------- restart -------------------------------- */
    ImGui::SameLine(ImGui::GetWindowWidth() - 25);
    if (ImGui::Button(ICON_FA_ARROW_ROTATE_LEFT, ImVec2(25, 25))) {
      if (!this->m_sim->is_paused()) { m_sim->playToggle(); }
      this->m_sim->restart();
    }

    /* -------------------------------- direction ------------------------------- */
    int dir = this->m_sim->is_forward() ? 1 : 0;
    const char* directions[2] = {ICON_FA_BACKWARD, ICON_FA_FORWARD};
    const char* direction = directions[dir];
    ImGui::SetNextItemWidth(35);
    ImGui::SliderInt("##direction", &dir, 0, 1, direction);
    if (this->m_sim->is_forward() != static_cast<bool>(dir == 1)) { this->m_sim->reverse(); }

    /* ----------------------------- skip timesteps ----------------------------- */
    ImGui::SameLine();
    int jmp{this->m_sim->get_jumpover()};
    ImGui::DragInt("##skip", &jmp, 1, 1, 1000, "t += %d dt");
    this->m_sim->set_jumpover(jmp);
  }

  // // Simulation speed
  // {
  //   ImGui::SetNextItemWidth(
  //       std::max(ImGui::GetContentRegionAvail().x * 0.5f, ImGui::GetFontSize() * 6));
  //   ImGui::SliderFloat("TPS", &this->m_tps_limit, 1, 1000);
  // }
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
  }
  if (glfwGetKey(this->m_window->get_window(), GLFW_KEY_RIGHT) == GLFW_PRESS) {
    this->m_sim->stepFwd();
  }

  static bool pressing_left = false;
  if (glfwGetKey(m_window->get_window(), GLFW_KEY_COMMA) == GLFW_PRESS) { pressing_left = true; }
  if (glfwGetKey(this->m_window->get_window(), GLFW_KEY_COMMA) == GLFW_RELEASE && (pressing_left)) {
    pressing_left = false;
    this->m_sim->stepBwd();
  }
  static bool pressing_right = false;
  if (glfwGetKey(this->m_window->get_window(), GLFW_KEY_PERIOD) == GLFW_PRESS) {
    pressing_right = true;
  }
  if (glfwGetKey(this->m_window->get_window(), GLFW_KEY_PERIOD) == GLFW_RELEASE
      && (pressing_right)) {
    pressing_right = false;
    this->m_sim->stepFwd();
  }
}

template <class T, ushort D>
void Visualization<T, D>::drawMainMenuBar() {
  ImGui::BeginMainMenuBar();

  if (ImGui::BeginMenu("add plot")) {
    if (ImGui::MenuItem("pcolor")) { addPcolor2d(0, 1); }
    if (ImGui::MenuItem("scatter")) { addScatter2d(); }
    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("state")) {
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
  ImGui::EndMainMenuBar();
}

template <class T, ushort D>
void Visualization<T, D>::loop() {
  PLOGD_(VISPLOGID) << "Starting Visualization loop.";
  double fps_limit{glfwGetTime()};
  double tps_limit{glfwGetTime()};

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
    this->m_shared_axes.X.Min = 0.0f;
    this->m_shared_axes.X.Max = (float)x1max;
    this->m_shared_axes.Y.Min = -(float)x1max;
    this->m_shared_axes.Y.Max = (float)x1max;
  } else {
    this->m_shared_axes.X.Min = (float)x1min;
    this->m_shared_axes.X.Max = (float)x1max;
    this->m_shared_axes.Y.Min = (float)x2min;
    this->m_shared_axes.Y.Max = (float)x2max;
  }

  while (!this->m_window->windowShouldClose()) {
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    if ((this->m_fps_limit <= 0.0f) || (glfwGetTime() >= fps_limit + 1.0f / this->m_fps_limit)) {
      this->m_window->processInput();
      this->processControllerInput();

      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

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
          ImGui::BeginChild("controls", ImVec2(150, 0), true);
          this->drawControls();
          ImGui::EndChild();
        }
        ImGui::SameLine();
        {
          ImGui::BeginGroup();
          ImPlot::BeginSubplots("##subplots",
                                (int)std::ceil(this->m_plots.size() / 3.0f),
                                std::fmin(this->m_plots.size(), 3),
                                ImVec2(-1, -1));
          for (std::size_t i{0}; i < this->m_plots.size(); ++i) {
            ImGui::PushID(i);
            close_plots.push_back(this->m_plots[i]->draw(this->m_shared_axes));
            ImGui::PopID();
          }
          ImPlot::EndSubplots();
          ImGui::EndGroup();
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

      fps_limit = glfwGetTime();
    }

    // advance the simulation
    if ((this->m_tps_limit <= 0.0f) || (glfwGetTime() >= tps_limit + 1.0f / this->m_tps_limit)) {
      Sim->updateData();
      tps_limit = glfwGetTime();
    }
  }
}
} // namespace nttiny

template class nttiny::Visualization<float, 2>;
template class nttiny::Visualization<double, 2>;
