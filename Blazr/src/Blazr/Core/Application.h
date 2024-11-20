#pragma once
#include "blzrpch.h"
#include "Core.h"
#include "Window.h"

#include "../Events/ApplicationEvent.h"
namespace Blazr {

class BLZR_API Application {
  public:
	Application();
	virtual ~Application();
	virtual void Run();
	/*virtual void RenderImGui();
	virtual void End();
	virtual void Begin();*/
	void onEvent(Event &e);

  private:
	bool m_Running = true;
	bool onWindowClose(WindowCloseEvent &e);

  protected:
	std::unique_ptr<Window> m_Window;
};

// To be defined in CLIENT
Application *CreateApplication();
} // namespace Blazr
