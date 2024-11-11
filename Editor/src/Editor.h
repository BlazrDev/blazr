#pragma once

#include "Blazr.h"
#include "Blazr/Renderer/FrameBuffer.h"
#include "Blazr/Scene/Scene.h"

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
	void RenderSceneToTexture();

  private:
	Renderer2D m_Renderer;
	Ref<FrameBuffer> m_GameFrameBuffer;
	Ref<Scene> m_Scene;
};

Application *CreateApplication() { return new Editor(); }

} // namespace Blazr
