#pragma once

#include "Blazr.h"
#include "Blazr/Renderer/FrameBuffer.h"
#include "Blazr/Scene/Scene.h"
#include "imgui.h"

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
	void renderTransformComponent(ImVec2 &cursorPos);
	void renderIdentificationComponent(ImVec2 &cursorPos);
	void renderSpriteComponent(ImVec2 &cursorPos);
	void renderPhysicsComponent(ImVec2 &cursorPos);

  private:
	Renderer2D m_Renderer;
	Ref<FrameBuffer> m_GameFrameBuffer;
	Ref<Scene> m_Scene;
};

Application *CreateApplication() { return new Editor(); }

} // namespace Blazr
