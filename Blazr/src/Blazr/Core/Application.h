#pragma once
#include "../../blzrpch.h"
#include "Core.h"
#include "Window.h"

namespace Blazr {

class BLZR_API Application {
public:
  Application();
  virtual ~Application();
  void Run();

private:
  std::unique_ptr<Window> m_Window;
  bool m_Running = true;
};

// To be defined in CLIENT
Application *CreateApplication();
} // namespace Blazr
