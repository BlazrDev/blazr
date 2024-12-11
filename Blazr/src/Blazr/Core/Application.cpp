#include "blzrpch.h"
#include "Application.h"
#include "Blazr/Events/ApplicationEvent.h"
#include "Blazr/Scene/Scene.h"
#include "Log.h"

namespace Blazr {

#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)
Application::Application() {
	m_Window = std::unique_ptr<Window>(Window::create());
	m_Window->setEventCallback(BIND_EVENT_FN(Application::onEvent));
	Initialize();
}

Application::~Application() {}

void Application::onEvent(Event &e) {
	// EventDispatcher dispatcher(e);
	// dispatcher.Dispatch<WindowCloseEvent>(
	// 	BIND_EVENT_FN(Application::onWindowClose));
	//
	// BLZR_CORE_INFO("{0}", e.toString());
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

void Application::Initialize() {

	BLZR_CORE_WARN("Initializing!");
	m_Registry = Registry::GetInstance();

	auto assetManager = AssetManager::GetInstance();
	auto soundPlayer = SoundPlayer::GetInstance();
	if (!assetManager) {
		BLZR_CORE_ERROR("Failed to create the asset manager!");
		return;
	}

	auto physicsWorld = std::make_shared<b2World>(b2Vec2(0.0f, -9.8f));

	if (!m_Registry->AddToContext<std::shared_ptr<b2World>>(physicsWorld)) {
		BLZR_CORE_ERROR("Failed to create the physics world!");
		return;
	}

	auto physicsSystem = std::make_shared<PhysicsSystem>(*m_Registry);
	if (!m_Registry->AddToContext<std::shared_ptr<PhysicsSystem>>(
			physicsSystem)) {
		BLZR_CORE_ERROR(
			"Failed to add the physics system to the registry context!");
		return;
	}

	m_LuaState = CreateRef<sol::state>();

	if (!m_LuaState) {
		BLZR_CORE_ERROR("Failed to create the lua state!");
		return;
	}

	m_LuaState->open_libraries(sol::lib::base, sol::lib::math, sol::lib::os,
							   sol::lib::table, sol::lib::io, sol::lib::string);

	if (!m_Registry->AddToContext<std::shared_ptr<sol::state>>(m_LuaState)) {
		BLZR_CORE_ERROR(
			"Failed to add the sol::state to the registry context!");
		return;
	}
	if (!m_Registry->AddToContext<std::shared_ptr<AssetManager>>(
			assetManager)) {
		BLZR_CORE_ERROR(
			"Failed to load the asset manager to the registry context!");
		return;
	}
	if (!m_Registry->AddToContext<std::shared_ptr<SoundPlayer>>(soundPlayer)) {
		BLZR_CORE_ERROR(
			"Failed to load the sound player to the registry context!");
		return;
	}

	auto scriptSystem = std::make_shared<ScriptingSystem>(*m_Registry);
	if (!scriptSystem) {
		BLZR_CORE_ERROR("Failed to create the script system!");
		return;
	}

	if (!m_Registry->AddToContext<std::shared_ptr<ScriptingSystem>>(
			scriptSystem)) {
		BLZR_CORE_ERROR(
			"Failed to add the script system to the registry context!");
		return;
	}

	Scene::BindScene(*m_LuaState);
	ScriptingSystem::RegisterLuaBindings(*m_LuaState, *m_Registry);
	ScriptingSystem::RegisterLuaFunctions(*m_LuaState);
	LayerManager::BindLayerManager(*m_LuaState);
	Layer::BindLayer(*m_LuaState);
	// bind editor
	m_LuaState->new_usertype<Application>("EditorInterface", "SetActiveScene",
										  &Application::SetActiveScene);

	// Bind the editor instance to Lua under the global 'editor'
	(*m_LuaState)["editor"] = this;

	if (!scriptSystem->LoadMainScript(*m_LuaState)) {
		BLZR_CORE_ERROR("Failed to load the main lua script");
		return;
	}
	// TODO should also initialize entity script when a new entity is created
	scriptSystem->InitializeEntityScripts(*m_LuaState);

	auto animationSystem = std::make_shared<AnimationSystem>(*m_Registry);
	if (!animationSystem) {
		BLZR_CORE_ERROR("Failed to create the animation system!");
		return;
	}

	if (!m_Registry->AddToContext<std::shared_ptr<AnimationSystem>>(
			animationSystem)) {
		BLZR_CORE_ERROR(
			"Failed to add the animation system to the registry context!");
		return;
	}
}

Application *CreateApplication() { return nullptr; }

void Application::SetActiveScene(Ref<Scene> scene) { m_ActiveScene = scene; }
} // namespace Blazr
