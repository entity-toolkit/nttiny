#ifndef NTTINY_PLOTS_1D_H
#define NTTINY_PLOTS_1D_H

#include "defs.h"
#include "api.h"
#include "plots.h"

namespace nttiny {

template <class T, ushort D>
struct TimePlot : public Ax<T, D> {
  TimePlot(int id) : Ax<T, D>(id) {}
  ~TimePlot() override = default;
  auto draw(ImPlotRect&, UISettings&) -> bool override;
  auto exportMetadata() -> PlotMetadata override;
  void importMetadata(const PlotMetadata&) override;
};

} // namespace nttiny

#endif // NTTINY_PLOTS_2D_H
