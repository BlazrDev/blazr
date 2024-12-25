#pragma once
#include "blzrpch.h"
#include "Blazr.h"
#include "Blazr/Ecs/Components/AnimationComponent.h"
#include "Blazr/Ecs/Components/Identification.h"
#include "Blazr/Ecs/Components/ScriptComponent.h"
#include "Blazr/Renderer/FrameBuffer.h"
#include "Blazr/Scene/Scene.h"
#include "Blazr/Scene/TilemapScene.h"
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
									   Identification &identification,
									   const std::string &layerName);
	void renderSpriteComponent(ImVec2 &cursorPos, SpriteComponent &sprite,
							   Identification &identification);
	void renderAnimationComponent(ImVec2 &cursorPos,
								  AnimationComponent &animation);

	void renderScriptComponent(ImVec2 &cursorPos, ScriptComponent &script,
							   const entt::entity &entity);
	void renderPhysicsComponent(ImVec2 &cursorPos, PhysicsComponent &physics);
	void renderBoxColliderComponent(ImVec2 &cursorPos,
									BoxColliderComponent &boxCollider);
	void renderTileMapSettings(ImVec2 &cursorPos, TilemapScene &tileMap);

	void setEventCallback(const Window::EventCallbackFn &callback);

	glm::vec3 selectedTileData;
	static bool clickedStop;

  private:
	Renderer2D m_Renderer;
	Ref<FrameBuffer> m_GameFrameBuffer;
	Window::EventCallbackFn m_EventCallback;
};

Application *CreateApplication() { return new Editor(); }

} // namespace Blazr
