#pragma once
#include "blzrpch.h"
#include "Blazr.h"
#include "Blazr/Ecs/Components/Identification.h"
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
	void renderTransformComponent(ImVec2 &cursorPos,
								  TransformComponent &transform);
	void renderIdentificationComponent(ImVec2 &cursorPos,
									   Identification &identification);
	void renderSpriteComponent(ImVec2 &cursorPos, SpriteComponent &sprite);
	void renderPhysicsComponent(ImVec2 &cursorPos, PhysicsComponent &physics);
	void renderBoxColliderComponent(ImVec2 &cursorPos,
									BoxColliderComponent &boxCollider);

	void setEventCallback(const Window::EventCallbackFn &callback);

	Ref<Scene> GetActiveScene() { return m_ActiveScene; }

  private:
	Renderer2D m_Renderer;
	Ref<FrameBuffer> m_GameFrameBuffer;
	Ref<Scene> m_ActiveScene;
	Window::EventCallbackFn m_EventCallback;
};

Application *CreateApplication() { return new Editor(); }

} // namespace Blazr
