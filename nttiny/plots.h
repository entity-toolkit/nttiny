#ifndef NTTINY_PLOTS_H
#define NTTINY_PLOTS_H

#include "defs.h"
#include "api.h"

#include <implot.h>

#include <string>

namespace nttiny {

  template <class T> class Ax {
  protected:
    SimulationAPI<T> *m_sim;
    const int m_ID;

  public:
    Ax(int id);
    virtual ~Ax() = 0;
    virtual void draw();
    virtual auto getId() -> int;
    void bindSimulation(SimulationAPI<T> *sim);
  };

  template <class T> class Plot2d : public Ax<T> {
  protected:
    float m_scale{1.0f};

  public:
    Plot2d(int id) : Ax<T>(id) {}
    ~Plot2d() override = default;
    void scale();
    void close();
  };

  template <class T> class Pcolor2d : public Plot2d<T> {
  protected:
    float m_sidebar_w{60}, m_cmap_h{225};
    bool m_log;
    float m_plot_size{350};
    float m_vmin, m_vmax;
    ImPlotColormap m_cmap{ImPlotColormap_Jet};
    int m_field_selected{0};

  public:
    Pcolor2d(int id, float vmin, float vmax)
        : Plot2d<T>(id), m_vmin(vmin), m_vmax(vmax) {}
    ~Pcolor2d() override = default;
    auto getId() -> int override { return this->m_ID; }
    void draw() override;
  };

  // TODO: 1d plot, linear, log linear and log log, multiple data

  // template<class T>
  // class Plot : virtual public Plot2d {
  // private:
  //   T *m_xvalues, *m_yvalues;
  //   int *n;
  // protected:
  //   float m_ratio;
  //   float m_plot_size{350};
  //   std::string m_xlabel, m_ylabel;
  // public:
  //   Plot(ImVec2 xlims, ImVec2 ylims, float ratio) : Plot2d(xlims, ylims),
  //   m_ratio(ratio) {} ~Plot() = default; void draw() override;
  // };
}

#endif
