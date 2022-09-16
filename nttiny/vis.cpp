#include "defs.h"
#include "vis.h"

#include "api.h"

#include "cousine.h"

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
  // initialize plog
  plog::Severity max_severity;
#ifdef VERBOSE
  max_severity = plog::verbose;
#elif DEBUG
  max_severity = plog::debug;
#else
  max_severity = plog::warning;
#endif
  plog::init<VISPLOGID>(max_severity, &m_console_appender);
  // initialize glfw
  glfwInit();
  // open window
  m_window = std::make_unique<Window>(m_win_width, m_win_height, "Nttiny", 0, resizable);
  // initialize imgui
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_None;
  try {
    ImGui::GetIO().Fonts->Clear();
    ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
        Cousine_compressed_data, Cousine_compressed_size, 12.0f);
    ImGui::GetIO().Fonts->Build();
  }
  catch (std::runtime_error& e) {
    PLOGW << "Warning: " << e.what();
  }
  ImGui::StyleColorsDark();
  ImGui::GetStyle().AntiAliasedLines = true;
  ImGui::GetStyle().AntiAliasedFill = true;
  ImGui::GetStyle().FrameRounding = 3.0f;
  ImGui::GetStyle().ChildRounding = 3.0f;
  ImGui::GetStyle().WindowRounding = 3.0f;
  ImGui_ImplGlfw_InitForOpenGL(m_window->get_window(), true);
  ImGui_ImplOpenGL3_Init("#version 150");
}

template <class T, ushort D>
Visualization<T, D>::~Visualization() {
  // deinitialize imgui
  PLOGD_(VISPLOGID) << "Destroying Visualization.";
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImPlot::DestroyContext();
  ImGui::DestroyContext();
  // deinitialize glfw
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
void Visualization<T, D>::buildController() {
  ImGui::BeginChild("simulation control");
  ImGui::Text("timestep: %d", this->m_sim->get_timestep());
  ImGui::Text("time: %f", this->m_sim->get_time());
  {
    ImGui::PushButtonRepeat(true);
    if (ImGui::ArrowButton("##left", ImGuiDir_Left)) {
      this->m_sim->stepBwd();
      if (!this->m_sim->is_paused()) { m_sim->playToggle(); }
    }
    ImGui::PopButtonRepeat();
  }
  // Toggle play/pause
  {
    ImGui::SameLine();
    if (ImGui::Button(this->m_sim->is_paused() ? "play" : "pause")) { this->m_sim->playToggle(); }
  }
  // Right step
  {
    ImGui::SameLine();
    ImGui::PushButtonRepeat(true);
    if (ImGui::ArrowButton("##right", ImGuiDir_Right)) {
      this->m_sim->stepFwd();
      if (!this->m_sim->is_paused()) { this->m_sim->playToggle(); }
    }
    ImGui::PopButtonRepeat();
  }

  // restart simulation
  {
    ImGui::SameLine(ImGui::GetWindowWidth() - 40);
    if (ImGui::Button("rst")) {
      if (!this->m_sim->is_paused()) { m_sim->playToggle(); }
      this->m_sim->restart();
    }
  }

  // Simulation speed
  {
    ImGui::SetNextItemWidth(
        std::max(ImGui::GetContentRegionAvail().x * 0.5f, ImGui::GetFontSize() * 6));
    ImGui::SliderFloat("TPS", &this->m_tps_limit, 1, 1000);

    // ImGui::Text("Jump over timesteps:");
    ImGui::SetNextItemWidth(
        std::max(ImGui::GetContentRegionAvail().x * 0.5f, ImGui::GetFontSize() * 3));
    int jmp{this->m_sim->get_jumpover()};
    ImGui::InputInt("skip", &jmp);
    this->m_sim->set_jumpover(jmp);
  }
  // Simulation direction
  {
    int dir = this->m_sim->is_forward() ? 1 : 0;
    const char* directions[2] = {"<<", ">>"};
    const char* direction = directions[dir];
    ImGui::SetNextItemWidth(
        std::max(ImGui::GetContentRegionAvail().x * 0.2f, ImGui::GetFontSize() * 4));
    ImGui::SliderInt(this->m_sim->is_forward() ? "fwd" : "bwd", &dir, 0, 1, direction);
    if (this->m_sim->is_forward() != static_cast<bool>(dir == 1)) { this->m_sim->reverse(); }
  }
  // Add plots
  {
    if (ImGui::Button("add 2d plot")) { addPcolor2d(0, 1); }
    if (ImGui::Button("add scatter plot")) { addScatter2d(); }
  }
  // Save state
  {
    if (ImGui::Button("save state")) {
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
  }
  // Load state
  {
    if (ImGui::Button("load state")) {
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
void Visualization<T, D>::loop() {
  PLOGD_(VISPLOGID) << "Starting Visualization loop.";
  double fps_limit{glfwGetTime()};
  double tps_limit{glfwGetTime()};

  float prev_scale = 0.f;

  while (!this->m_window->windowShouldClose()) {
    float xscale, yscale;
    glfwGetWindowContentScale(this->m_window->get_window(), &xscale, &yscale);
    if (xscale != prev_scale) {
      prev_scale = xscale;
      ImGui::GetIO().Fonts->Clear();
      // ImGui::GetIO().Fonts->AddFontFromFileTTF("Roboto-Regular.ttf", xscale * 16.0f);
      ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
          Cousine_compressed_data, Cousine_compressed_size, xscale * 8.0f);
      ImGui::GetIO().Fonts->Build();
      ImGui_ImplOpenGL3_DestroyFontsTexture();
      ImGui_ImplOpenGL3_CreateFontsTexture();

      // ImGui::GetStyle().ScaleAllSizes(xscale);
    }

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    if ((this->m_fps_limit <= 0.0f) || (glfwGetTime() >= fps_limit + 1.0f / this->m_fps_limit)) {
      this->m_window->processInput();
      this->processControllerInput();

      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      // render all the plots
      std::vector<bool> close_plots;

      static bool use_work_area = false;
      static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove
                                    | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;
      const ImGuiViewport* viewport = ImGui::GetMainViewport();
      ImGui::SetNextWindowPos(use_work_area ? viewport->WorkPos : viewport->Pos);
      ImGui::SetNextWindowSize(use_work_area ? viewport->WorkSize : viewport->Size);
      bool open = true;

      if (ImGui::Begin("main dashboard", &open, flags)) {
        {
          ImGui::BeginChild("controls", ImVec2(150, 0), true);
          buildController();
          ImGui::EndChild();
        }
        ImGui::SameLine();
        // if (ImPlot::BeginSubplots("##Subplots",
        //                                  this->m_plots.size(),
        //                                  1,
        //                                  ImVec2(-1, -1),
        //                                  ImGuiWindowFlags_NoScrollbar)) {
        {
          ImGui::BeginGroup();
          ImPlot::BeginSubplots("##subplots",
                                (int)std::ceil(this->m_plots.size() / 3.0f),
                                std::fmin(this->m_plots.size(), 3),
                                ImVec2(-1, -1));
          for (std::size_t i{0}; i < this->m_plots.size(); ++i) {
            // auto plot = ;
            ImGui::PushID(i);
            close_plots.push_back(this->m_plots[i]->draw());
            ImGui::PopID();
          }
          ImPlot::EndSubplots();
          ImGui::EndGroup();
        }
        // }
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
      this->m_sim->updateData();
      tps_limit = glfwGetTime();
    }
  }
}
} // namespace nttiny

template class nttiny::Visualization<float, 2>;
template class nttiny::Visualization<double, 2>;
