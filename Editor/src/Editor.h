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
};

Application *CreateApplication() { return new Editor(); }

} // namespace Blazr
