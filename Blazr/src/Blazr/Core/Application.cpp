#include "Application.h"
#include "../Events/ApplicationEvent.h"
#include "Log.h"

namespace Blazr {
Application::Application() {
  m_Window = std::unique_ptr<Window>(Window::create());
}

Application::~Application() {}

void Application::Run() {

  while (m_Running) {
    m_Window->onUpdate();
  }
}
} // namespace Blazr
