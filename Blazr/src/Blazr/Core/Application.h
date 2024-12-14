#pragma once
#include "blzrpch.h"
#include "Blazr/Core/Log.h"
#include "Blazr/Ecs/Components/BoxColliderComponent.h"
#include "Blazr/Ecs/Components/PhysicsComponent.h"
#include "Blazr/Ecs/Components/SpriteComponent.h"
#include "Blazr/Ecs/Components/TransformComponent.h"
#include "Blazr/Ecs/Entity.h"
#include "Blazr/Renderer/Renderer2D.h"
#include "Blazr/Resources/AssetManager.h"
#include "Blazr/Scene/Scene.h"
#include "Blazr/Systems/AnimationSystem.h"
#include "Blazr/Systems/PhysicsSystem.h"
#include "Blazr/Systems/ScriptingSystem.h"
#include "Blazr/Systems/Sounds/SoundPlayer.h"
#include "Core.h"
#include "Window.h"

#include "../Events/ApplicationEvent.h"
namespace Blazr {

class BLZR_API Application {
  public:
	Application();
	virtual ~Application();
	virtual void Run();
	void Initialize();
	/*virtual void RenderImGui();
	virtual void End();
	virtual void Begin();*/
	void onEvent(Event &e);
	void SetActiveScene(Ref<Scene> scene);
	Ref<Scene> GetActiveScene() { return m_ActiveScene; }

  private:
	bool m_Running = true;
	bool onWindowClose(WindowCloseEvent &e);

  protected:
	std::unique_ptr<Window> m_Window;
	std::shared_ptr<Blazr::Registry> m_Registry;
	Ref<sol::state> m_LuaState;
	Ref<Scene> m_ActiveScene;
};

// To be defined in CLIENT
Application *CreateApplication();
} // namespace Blazr
