#include "blzrpch.h"
#include "Blazr/Renderer/Renderer2D.h"
#include "Blazr/Resources/AssetManager.h"
#include "Blazr/Systems/AnimationSystem.h"
#include "Blazr/Systems/ScriptingSystem.h"
#include "Scene.h"

namespace Blazr {

Scene::Scene() : m_Camera(1280, 720) {
	m_Camera.SetScale(1.0f);
	m_Camera.SetPosition({0.0f, 0.0f});
	auto assetManager = AssetManager::GetInstance();
	m_Registry = std::make_shared<Registry>();

	if (!assetManager) {
		BLZR_CORE_ERROR("Failed to create the asset manager!");
		return;
	}

	if (!assetManager->LoadTexture("chammy", "assets/chammy.png", false)) {
		BLZR_CORE_ERROR("Failed to load the chammy texture!");
		return;
	}

	if (!assetManager->LoadTexture("masha", "assets/masha.png", false)) {
		BLZR_CORE_ERROR("Failed to load the masha texture!");
		return;
	}

	if (!assetManager->LoadTexture("player", "assets/sprite_sheet.png",
								   false)) {
		BLZR_CORE_ERROR("Failed to load the chammy texture!");
		return;
	}

	auto playerTexture = assetManager->GetTexture("player");
	auto mashaTexture = assetManager->GetTexture("masha");

	auto lua = std::make_shared<sol::state>();

	if (!lua) {
		BLZR_CORE_ERROR("Failed to create the lua state!");
		return;
	}

	lua->open_libraries(sol::lib::base, sol::lib::math, sol::lib::os,
						sol::lib::table, sol::lib::io, sol::lib::string);

	if (!m_Registry->AddToContext<std::shared_ptr<sol::state>>(lua)) {
		BLZR_CORE_ERROR(
			"Failed to add the sol::state to the registry context!");
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

	ScriptingSystem::RegisterLuaBindings(*lua, *m_Registry);
	if (!scriptSystem->LoadMainScript(*lua)) {
		BLZR_CORE_ERROR("Failed to load the main lua script");
		return;
	}

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

Scene::~Scene() {}

entt::entity Scene::AddEntity() { return m_Registry->CreateEntity(); }

void Scene::RemoveEntity(entt::entity entity) {

	m_Registry->GetRegistry().destroy(entity);
}

void Scene::Update() {

	if (auto animationSystem =
			m_Registry->GetContext<std::shared_ptr<AnimationSystem>>()) {
		animationSystem->Update();
	}

	if (auto scriptingSystem =
			m_Registry->GetContext<std::shared_ptr<ScriptingSystem>>()) {
		scriptingSystem->Update();
		scriptingSystem->Render();
	}
}

void Scene::Render() {

	Renderer2D::BeginScene(m_Camera);

	auto view =
		m_Registry->GetRegistry().view<TransformComponent, SpriteComponent>();
	for (auto entity : view) {
		auto &transform = view.get<TransformComponent>(entity);
		auto &sprite = view.get<SpriteComponent>(entity);

		sprite.generateTextureCoordinates();

		Renderer2D::DrawQuad(*m_Registry, entity);
	}

	Renderer2D::EndScene();
	Renderer2D::Flush();
}

} // namespace Blazr
