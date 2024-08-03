#pragma once

#include "Blazr/Core/Window.h"
#include "blzrpch.h"
namespace Blazr {
class BLZR_API LinuxWindow : public Window {
public:
  LinuxWindow(const WindowProperties &properties);
  virtual ~LinuxWindow() override;

  void onUpdate() override;
  unsigned int getWidth() const override;
  unsigned int getHeight() const override;
  void setEventCallback(const EventCallbackFn &callback) override;
  void setVSync(bool enabled) override;
  bool isVSync() const override;

private:
  virtual void init(const WindowProperties &properties);
  virtual void shutdown();

private:
  GLFWwindow *m_Window;

  // Need window data to pass into GLFW for events
  struct WindowData {
    std::string title;
    unsigned int width, height;
    bool vsync;

    EventCallbackFn eventCallback;
  };

  WindowData m_Data;
};
} // namespace Blazr
