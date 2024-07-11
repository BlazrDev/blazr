#pragma once
#include "Core.h"
#include "Window.h"

#include "../Events/ApplicationEvent.h"
namespace Blazr {

class BLZR_API Application {
public:
  Application();
  virtual ~Application();
  void Run();
  void onEvent(Event &e);

private:
  std::unique_ptr<Window> m_Window;
  bool m_Running = true;
  bool onWindowClose(WindowCloseEvent &e);
};

// To be defined in CLIENT
Application *CreateApplication();
} // namespace Blazr
