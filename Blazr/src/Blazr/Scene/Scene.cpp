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
#include "Scene.h"
#include "box2d/box2d.h"

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

	if (!assetManager->LoadTexture("texture", "assets/texture.png", false)) {
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

	// Create the player entity

	// glm::vec2 pos = {100.f, 100.f};
	// // glm::vec2 size = {200.f, 200.f};
	// glm::vec4 color = {1.f, 1.f, 1.f, 1.f};
	//
	// Entity entity = Entity(*m_Registry, "Ent1", "G1");
	// auto &transform =
	// 	entity.AddComponent<TransformComponent>(TransformComponent{
	// 		.position = pos, .scale = glm::vec2(0.3f, 0.3f), .rotation = 0.0f});
	//
	// auto &sprite = entity.AddComponent<SpriteComponent>(
	// 	SpriteComponent{.width = 472.f,
	// 					.height = 617.f,
	// 					.startX = 0,
	// 					.startY = 0,
	// 					.texturePath = "masha"});
	// auto &collider =
	// 	entity.AddComponent<BoxColliderComponent>(BoxColliderComponent{
	// 		.width = 472, .height = 617, .offset = glm::vec2(0, 0)});
	// sprite.generateObject(472, 617);
	//
	// auto &physics = entity.AddComponent<PhysicsComponent>(
	// 	PhysicsComponent{physicsWorld,
	// 					 PhysicsAtributes{
	// 						 .type = RigidBodyType::DYNAMIC,
	// 						 .density = 100.f,
	// 						 .friction = 0.5f,
	// 						 .restitution = 0.4f,
	// 						 .radius = 0.f,
	// 						 .gravityScale = 9.81f,
	// 						 .position = transform.position,
	// 						 .scale = transform.scale,
	// 						 .isFixedRotation = true,
	// 						 .isCircle = false,
	// 						 .isBoxShape = true,
	// 					 }} // namespace Blazr
	// );
	// physics.init(1280, 720);
	//
	// glm::vec2 pos2 = {100.f, 600.f};
	// // glm::vec2 size = {200.f, 200.f};
	//
	// Entity entity2 = Entity(*m_Registry, "Ent2", "G2");
	// auto &transform2 =
	// 	entity2.AddComponent<TransformComponent>(TransformComponent{
	// 		.position = pos2, .scale = glm::vec2(1.f, 1.f), .rotation = 0.0f});
	//
	// auto &sprite2 = entity2.AddComponent<SpriteComponent>(
	// 	SpriteComponent{.width = 224.f,
	// 					.height = 224.f,
	// 					.startX = 0,
	// 					.startY = 0,
	// 					.texturePath = "chammy"});
	// auto &collider2 =
	// 	entity2.AddComponent<BoxColliderComponent>(BoxColliderComponent{
	// 		.width = 224, .height = 224, .offset = glm::vec2(0, 0)});
	// sprite2.generateObject(224, 224);
	// //
	// auto &physics2 = entity2.AddComponent<PhysicsComponent>(
	// 	PhysicsComponent{physicsWorld,
	// 					 PhysicsAtributes{
	// 						 .type = RigidBodyType::STATIC,
	// 						 .density = 100.f,
	// 						 .friction = 0.9f,
	// 						 .restitution = 0.f,
	// 						 .radius = 0.f,
	// 						 .gravityScale = 5.f,
	// 						 .position = transform2.position,
	// 						 .scale = transform2.scale,
	// 						 .isFixedRotation = true,
	// 						 .isCircle = false,
	// 						 .isBoxShape = true,
	// 					 }} // namespace Blazr
	// );
	//
	// physics2.init(1280, 720);
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
		// Renderer2D::DrawQuad(entt::null,
		// 					 {transform.position.x + collider.offset.x,
		// 					  transform.position.y + collider.offset.y},
		// 					 {collider.width * transform.scale.x,
		// 					  collider.height * transform.scale.y},
		// 					 {255, 0, 0, 0.6});
	}

	Renderer2D::EndScene();
	Renderer2D::Flush();
}

} // namespace Blazr
