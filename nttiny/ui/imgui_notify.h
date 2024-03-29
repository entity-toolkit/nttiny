// adapted from `imgui-notify` by patrickcjk
// https://github.com/patrickcjk/imgui-notify

#ifndef IMGUI_NOTIFY
#define IMGUI_NOTIFY

#include "icons.h"

#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <chrono>
#include <string>
#include <vector>

#define NOTIFY_MAX_MSG_LENGTH    4096    // Max message content length
#define NOTIFY_PADDING_X         20.f    // Bottom-left X padding
#define NOTIFY_PADDING_Y         20.f    // Bottom-left Y padding
#define NOTIFY_PADDING_MESSAGE_Y 10.f    // Padding Y between each message
#define NOTIFY_FADE_IN_OUT_TIME  150     // Fade in and out duration
#define NOTIFY_DEFAULT_DISMISS                                                                \
  3000    // Auto dismiss after X ms (default, applied only of no data provided in
          // constructors)
#define NOTIFY_OPACITY 1.0f    // 0-1 Toast opacity
#define NOTIFY_TOAST_FLAGS                                                                    \
  ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration                           \
    | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoNav                                      \
    | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing
// Comment out if you don't want any separator between title and content
#define NOTIFY_USE_SEPARATOR
#define NOTIFY_INLINE             inline
#define NOTIFY_NULL_OR_EMPTY(str) (!str || !strlen(str))
#define NOTIFY_FORMAT(fn, format)                                                             \
  if (format) {                                                                               \
    va_list args;                                                                             \
    va_start(args, format);                                                                   \
    fn(format, args);                                                                         \
    va_end(args);                                                                             \
  }

enum ImGuiToastType {
  ImGuiToastType_None,
  ImGuiToastType_Success,
  ImGuiToastType_Warning,
  ImGuiToastType_Error,
  ImGuiToastType_Info,
  ImGuiToastType_COUNT
};

enum ImGuiToastPhase {
  ImGuiToastPhase_FadeIn,
  ImGuiToastPhase_Wait,
  ImGuiToastPhase_FadeOut,
  ImGuiToastPhase_Expired,
  ImGuiToastPhase_COUNT
};

enum ImGuiToastPos {
  ImGuiToastPos_TopLeft,
  ImGuiToastPos_TopCenter,
  ImGuiToastPos_TopRight,
  ImGuiToastPos_BottomLeft,
  ImGuiToastPos_BottomCenter,
  ImGuiToastPos_BottomRight,
  ImGuiToastPos_Center,
  ImGuiToastPos_COUNT
};

class ImGuiToast {
private:
  ImGuiToastType type = ImGuiToastType_None;
  char           title[NOTIFY_MAX_MSG_LENGTH];
  char           content[NOTIFY_MAX_MSG_LENGTH];
  unsigned long  dismiss_time = NOTIFY_DEFAULT_DISMISS;
  std::chrono::time_point<std::chrono::high_resolution_clock> creation_time
    = std::chrono::high_resolution_clock::now();

private:
  // Setters

  NOTIFY_INLINE auto set_title(const char* format, va_list args) {
    vsnprintf(this->title, sizeof(this->title), format, args);
  }

  NOTIFY_INLINE auto set_content(const char* format, va_list args) {
    vsnprintf(this->content, sizeof(this->content), format, args);
  }

public:
  NOTIFY_INLINE auto set_title(const char* format, ...) -> void {
    NOTIFY_FORMAT(this->set_title, format);
  }

  NOTIFY_INLINE auto set_content(const char* format, ...) -> void {
    NOTIFY_FORMAT(this->set_content, format);
  }

  NOTIFY_INLINE auto set_type(const ImGuiToastType& type) -> void {
    IM_ASSERT(type < ImGuiToastType_COUNT);
    this->type = type;
  };

public:
  // Getters

  NOTIFY_INLINE auto get_title() const -> const char* {
    return this->title;
  };

  NOTIFY_INLINE auto get_default_title() const -> const char* {
    if (!strlen(this->title)) {
      switch (this->type) {
      case ImGuiToastType_Success:
        return "Success";
      case ImGuiToastType_Warning:
        return "Warning";
      case ImGuiToastType_Error:
        return "Error";
      case ImGuiToastType_Info:
        return "Info";
      default:
        return nullptr;
      }
    }

    return this->title;
  };

  NOTIFY_INLINE auto get_type() const -> ImGuiToastType {
    return this->type;
  };

  NOTIFY_INLINE auto get_color() const -> ImVec4 {
    switch (this->type) {
    case ImGuiToastType_None:
      return { 255, 255, 255, 255 };    // White
    case ImGuiToastType_Success:
      return { 0, 255, 0, 255 };    // Green
    case ImGuiToastType_Warning:
      return { 255, 255, 0, 255 };    // Yellow
    case ImGuiToastType_Error:
      return { 255, 0, 0, 255 };    // Error
    case ImGuiToastType_Info:
      return { 0, 157, 255, 255 };    // Blue
    default:
      return { 255, 255, 255, 255 };    // White
    }
  }

  NOTIFY_INLINE auto get_icon() const -> const char* {
    switch (this->type) {
    case ImGuiToastType_None:
      return nullptr;
    case ImGuiToastType_Success:
      return ICON_FA_CIRCLE_CHECK;
    case ImGuiToastType_Warning:
      return ICON_FA_TRIANGLE_EXCLAMATION;
    case ImGuiToastType_Error:
      return ICON_FA_XMARK;
    case ImGuiToastType_Info:
      return ICON_FA_CIRCLE_INFO;
    default:
      return nullptr;
    }
  }

  NOTIFY_INLINE auto get_content() const -> const char* {
    return this->content;
  };

  NOTIFY_INLINE auto get_elapsed_time() const -> unsigned long int {
    unsigned long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                              std::chrono::high_resolution_clock::now() - this->creation_time)
                              .count();
    return elapsed;
  }

  NOTIFY_INLINE auto get_phase() const -> ImGuiToastPhase {
    const auto elapsed = get_elapsed_time();

    if (elapsed > NOTIFY_FADE_IN_OUT_TIME + this->dismiss_time + NOTIFY_FADE_IN_OUT_TIME) {
      return ImGuiToastPhase_Expired;
    } else if (elapsed > NOTIFY_FADE_IN_OUT_TIME + this->dismiss_time) {
      return ImGuiToastPhase_FadeOut;
    } else if (elapsed > NOTIFY_FADE_IN_OUT_TIME) {
      return ImGuiToastPhase_Wait;
    } else {
      return ImGuiToastPhase_FadeIn;
    }
  }

  NOTIFY_INLINE auto get_fade_percent() const -> float {
    const auto phase   = get_phase();
    const auto elapsed = get_elapsed_time();

    if (phase == ImGuiToastPhase_FadeIn) {
      return ((float)elapsed / (float)NOTIFY_FADE_IN_OUT_TIME) * NOTIFY_OPACITY;
    } else if (phase == ImGuiToastPhase_FadeOut) {
      return (1.f
              - (((float)elapsed - (float)NOTIFY_FADE_IN_OUT_TIME - (float)this->dismiss_time)
                 / (float)NOTIFY_FADE_IN_OUT_TIME))
             * NOTIFY_OPACITY;
    }

    return 1.f * NOTIFY_OPACITY;
  }

public:
  // Constructors

  ImGuiToast(ImGuiToastType type, int dismiss_time = NOTIFY_DEFAULT_DISMISS) {
    IM_ASSERT(type < ImGuiToastType_COUNT);

    this->type          = type;
    this->dismiss_time  = dismiss_time;
    this->creation_time = std::chrono::high_resolution_clock::now();

    memset(this->title, 0, sizeof(this->title));
    memset(this->content, 0, sizeof(this->content));
  }

  ImGuiToast(ImGuiToastType type, const char* format, ...) : ImGuiToast(type) {
    NOTIFY_FORMAT(this->set_content, format);
  }

  ImGuiToast(ImGuiToastType type, int dismiss_time, const char* format, ...)
    : ImGuiToast(type, dismiss_time) {
    NOTIFY_FORMAT(this->set_content, format);
  }
};

namespace ImGui {
  NOTIFY_INLINE std::vector<ImGuiToast> notifications;

  /// <summary>
  /// Insert a new toast in the list
  /// </summary>
  NOTIFY_INLINE void                    InsertNotification(const ImGuiToast& toast) {
    notifications.push_back(toast);
  }

  /// <summary>
  /// Remove a toast from the list by its index
  /// </summary>
  /// <param name="index">index of the toast to remove</param>
  NOTIFY_INLINE void RemoveNotification(int index) {
    notifications.erase(notifications.begin() + index);
  }

  /// <summary>
  /// Render toasts, call at the end of your rendering!
  /// </summary>
  NOTIFY_INLINE void RenderNotifications() {
    const auto vp_size = GetMainViewport()->Size;

    PushStyleColor(ImGuiCol_WindowBg,
                   ImVec4(43.f / 255.f, 43.f / 255.f, 43.f / 255.f, 100.f / 255.f));

    float height = 0.f;

    for (std::size_t i = 0; i < notifications.size(); i++) {
      auto* current_toast = &notifications[i];

      // Remove toast if expired
      if (current_toast->get_phase() == ImGuiToastPhase_Expired) {
        RemoveNotification(i);
        continue;
      }

      // Get icon, title and other data
      const auto icon          = current_toast->get_icon();
      const auto title         = current_toast->get_title();
      const auto content       = current_toast->get_content();
      const auto default_title = current_toast->get_default_title();
      const auto opacity    = current_toast->get_fade_percent();    // Get opacity based of the
                                                                    // current phase

      // Window rendering
      auto       text_color = current_toast->get_color();
      text_color.w          = opacity;

      // PushStyleColor(ImGuiCol_Text, text_color);
      SetNextWindowBgAlpha(opacity);
      SetNextWindowPos(
        ImVec2(vp_size.x - NOTIFY_PADDING_X, vp_size.y - NOTIFY_PADDING_Y - height),
        ImGuiCond_Always,
        ImVec2(1.0f, 1.0f));
      Begin(("##TOAST%d" + std::to_string(i)).c_str(), NULL, NOTIFY_TOAST_FLAGS);
      BringWindowToDisplayFront(GetCurrentWindow());

      // Here we render the toast content
      {
        PushTextWrapPos(vp_size.x / 3.f);    // We want to support multi-line text, this will
                                             // wrap the text after 1/3 of the screen width

        bool was_title_rendered = false;

        // If an icon is set
        if (!NOTIFY_NULL_OR_EMPTY(icon)) {
          // Text(icon); // Render icon text
          TextColored(text_color, "%s", icon);
          was_title_rendered = true;
        }

        // If a title is set
        if (!NOTIFY_NULL_OR_EMPTY(title)) {
          // If a title and an icon is set, we want to render on same line
          if (!NOTIFY_NULL_OR_EMPTY(icon))
            SameLine();

          Text("%s", title);    // Render title text
          was_title_rendered = true;
        } else if (!NOTIFY_NULL_OR_EMPTY(default_title)) {
          if (!NOTIFY_NULL_OR_EMPTY(icon))
            SameLine();

          Text("%s", default_title);    // Render default title text (ImGuiToastType_Success ->
                                        // "Success", etc...)
          was_title_rendered = true;
        }

        // In case ANYTHING was rendered in the top, we want to add a small padding so the text
        // (or icon) looks centered vertically
        if (was_title_rendered && !NOTIFY_NULL_OR_EMPTY(content)) {
          SetCursorPosY(GetCursorPosY() + 5.f);    // Must be a better way to do this!!!!
        }

        // If a content is set
        if (!NOTIFY_NULL_OR_EMPTY(content)) {
          if (was_title_rendered) {
#ifdef NOTIFY_USE_SEPARATOR
            Separator();
#endif
          }

          Text("%s", content);    // Render content text
        }

        PopTextWrapPos();
      }

      // Save height for next toasts
      height += GetWindowHeight() + NOTIFY_PADDING_MESSAGE_Y;

      // End
      End();
    }
    ImGui::PopStyleColor(1);
  }

}    // namespace ImGui

#endif
