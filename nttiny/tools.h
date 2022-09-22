#ifndef NTTINY_TOOLS_H
#define NTTINY_TOOLS_H

#include "defs.h"

#include <imgui.h>

namespace nttiny::tools {
void drawCircle(const ImVec2& center,
                const float& radius,
                const ImVec2& phi_range = {0.0f, 2.0f * M_PI},
                const int& resolution = 128,
                const ImU32& color = IM_COL32(255, 255, 255, 255));
}

#endif