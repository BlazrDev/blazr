#pragma once
#include "Blazr.h"

namespace Blazr {
class Editor : public Application {
  public:
	Editor();
	~Editor() override;

	void Init();
	void InitImGui();
	void Run() override;
	void RenderImGui();
	void Shutdown();

  private:
	Renderer2D m_Renderer;
	Camera2D m_Camera;
	Window *m_Window;
};

Application *CreateApplication() { return new Editor(); }

} // namespace Blazr
