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
	void Shutdown();
	void RenderImGui();

	void End();
	void Begin();
	void RenderSceneToTexture();
	void setEventCallback(const Window::EventCallbackFn &callback);

  private:
	Renderer2D m_Renderer;
	Ref<FrameBuffer> m_GameFrameBuffer;
	Ref<Scene> m_Scene;
	Window::EventCallbackFn m_EventCallback;
};

Application *CreateApplication() { return new Editor(); }

} // namespace Blazr
