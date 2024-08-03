// clang-format off
#include "../../blzrpch.h"
#include "Application.h"
#include "Blazr/Events/ApplicationEvent.h"
#include "Log.h"
// clang-format on

namespace Blazr {

#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)
Application::Application() {
  m_Window = std::unique_ptr<Window>(Window::create());
  m_Window->setEventCallback(BIND_EVENT_FN(Application::onEvent));
}

Application::~Application() {}

void Application::onEvent(Event &e) {
  EventDispatcher dispatcher(e);
  dispatcher.Dispatch<WindowCloseEvent>(
      BIND_EVENT_FN(Application::onWindowClose));

  BLZR_CORE_INFO("{0}", e.toString());
}

bool Application::onWindowClose(WindowCloseEvent &e) {
  m_Running = false;
  return true;
}
void Application::Run() {

  // Render loop
  while (m_Running) {
    m_Window->onUpdate();
  }
}
} // namespace Blazr
