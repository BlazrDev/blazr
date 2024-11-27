#include "blzrpch.h"
#include "Blazr/Core/Log.h"
#include "Blazr/Ecs/Components/BoxColliderComponent.h"
#include "Blazr/Ecs/Components/PhysicsComponent.h"
#include "Blazr/Ecs/Components/SpriteComponent.h"
#include "Blazr/Ecs/Components/TransformComponent.h"
#include "Blazr/Ecs/Entity.h"
#include "Blazr/Renderer/Renderer2D.h"
#include "Blazr/Resources/AssetManager.h"
#include "Blazr/Systems/AnimationSystem.h"
#include "Blazr/Systems/PhysicsSystem.h"
#include "Blazr/Systems/ScriptingSystem.h"
#include "Blazr/Systems/Sounds/SoundPlayer.h"
#include "Scene.h"
#include "box2d/box2d.h"
#include <json.hpp>

using namespace nlohmann;
namespace Blazr {

Scene::Scene() : m_Camera(1280, 720) {
	m_Camera.SetScale(1.0f);
	m_Camera.SetPosition({0.0f, 0.0f});
	auto assetManager = AssetManager::GetInstance();
	auto soundPlayer = SoundPlayer::GetInstance();
	m_Registry = std::make_shared<Registry>();

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

	ScriptingSystem::RegisterLuaBindings(*lua, *m_Registry);
	ScriptingSystem::RegisterLuaFunctions(*lua);
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

	auto &physicsWorld = m_Registry->GetContext<std::shared_ptr<b2World>>();
	physicsWorld->Step(1.0f / 60.0f, 10, 8);
	if (auto physicsSystem =
			m_Registry->GetContext<std::shared_ptr<PhysicsSystem>>()) {
		physicsSystem->Update(*m_Registry);
	}
}

void Scene::Render() {

	Renderer2D::BeginScene(m_Camera);
	m_Camera.Update();

	auto view =
		m_Registry->GetRegistry()
			.view<TransformComponent, SpriteComponent, BoxColliderComponent>();
	for (auto entity : view) {
		auto &transform = view.get<TransformComponent>(entity);
		auto &sprite = view.get<SpriteComponent>(entity);
		auto &collider = view.get<BoxColliderComponent>(entity);

		sprite.generateTextureCoordinates();

		Renderer2D::DrawQuad(*m_Registry, entity);
		Renderer2D::DrawQuad(entt::null,
							 {transform.position.x + collider.offset.x,
							  transform.position.y + collider.offset.y},
							 {collider.width * transform.scale.x,
							  collider.height * transform.scale.y},
							 {255, 0, 0, 0.6});
	}

	Renderer2D::EndScene();
	Renderer2D::Flush();
}

void Scene::Serialize(nlohmann::json &j) const {
	j["Entities"] = nlohmann::json::array();
	auto &registry = m_Registry->GetRegistry();

	auto view = registry.view<TransformComponent, SpriteComponent>();

	view.each([&](auto entity, TransformComponent &transform,
				  SpriteComponent &sprite) {
		nlohmann::json entityJson;

		TransformComponent::to_json(entityJson["Transform"], transform);

		SpriteComponent::to_json(entityJson["Sprite"], sprite);

		j["Entities"].push_back(entityJson);
	});
}

void Scene::Deserialize(const nlohmann::json &j) {
	auto &registry = m_Registry->GetRegistry();

	for (const auto &entityJson : j.at("Entities")) {
		auto entity = registry.create();

		if (entityJson.contains("Transform")) {
			TransformComponent transform;
			TransformComponent::from_json(entityJson.at("Transform"),
										  transform);
			registry.emplace<TransformComponent>(entity, transform);
		}

		if (entityJson.contains("Sprite")) {
			SpriteComponent sprite;
			SpriteComponent::from_json(entityJson.at("Sprite"), sprite);
			registry.emplace<SpriteComponent>(entity, sprite);
		}
	}
}

} // namespace Blazr
