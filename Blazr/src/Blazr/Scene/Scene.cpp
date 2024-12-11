#include "blzrpch.h"
#include "Blazr/Core/Log.h"
#include "Blazr/Ecs/Components/BoxColliderComponent.h"
#include "Blazr/Ecs/Components/PhysicsComponent.h"
#include "Blazr/Ecs/Components/SpriteComponent.h"
#include "Blazr/Ecs/Components/TransformComponent.h"
#include "Blazr/Ecs/Entity.h"
#include "Blazr/Renderer/CameraController.h"
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

Scene::Scene() : m_Camera(1280.0f, 720.f, true) {
	m_Camera.GetCamera().SetScale(1.0f);
	m_Camera.GetCamera().SetPosition({0.0f, 0.0f});
	m_Camera.GetCamera().SetRotation(0.0f);
	m_LayerManager = CreateRef<LayerManager>();
	m_Registry = Registry::GetInstance();
	m_LayerManager->CreateLayer("Layer 1", 0);
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

void Scene::onEvent(Event &e) { m_Camera.OnEvent(e); }

void Scene::Render() {

	Renderer2D::BeginScene(m_Camera.GetCamera());
	m_Camera.GetCamera().Update();
	m_Camera.OnUpdate();

	m_LayerManager->Render(*m_Registry);

	Renderer2D::EndScene();
	Renderer2D::Flush();
}

void Scene::Serialize(nlohmann::json &j) const {
	j["Name"] = m_Name; // Serialize the name of the scene

	j["Layers"] = nlohmann::json::array();
	auto layers = m_LayerManager->GetAllLayers();
	std::for_each(layers.begin(), layers.end(), [&](Ref<Layer> layer) {
		nlohmann::json layerJson;
		Layer::to_json(layerJson[layer->name], layer);
		j["Layers"].push_back(layerJson);
	});
}

void Scene::Deserialize(const nlohmann::json &j) {
	if (j.contains("Name")) {
		m_Name = j.at("Name").get<std::string>();
	} else {
		BLZR_CORE_WARN(
			"Scene JSON does not contain 'Name'. Using default name.");
	}

	if (j.contains("Layers")) {
		for (const auto &layerJson : j.at("Layers")) {
			for (const auto &[layerName, layerData] : layerJson.items()) {
				Ref<Layer> layer = CreateRef<Layer>(layerName, 0);
				Layer::from_json(layerData, layer);
				m_LayerManager->AddLayer(layer);
			}
		}
	}
}

void Scene::AddLayer(const std::string &layerName, int zIndex) {
	m_LayerManager->CreateLayer(layerName, zIndex);
}

void Scene::RemoveLayer(const std::string &layerName) {
	m_LayerManager->RemoveLayer(layerName);
}

Ref<Layer> Scene::GetLayerByName(const std::string &layerName) {
	return m_LayerManager->GetLayerByName(layerName);
}

std::vector<Ref<Layer>> Scene::GetAllLayers() const {
	return m_LayerManager->GetAllLayers();
}

void Scene::AddEntityToLayer(const std::string &layerName, Ref<Entity> entity) {
	Ref<Layer> layer = m_LayerManager->GetLayerByName(layerName);
	if (layer) {
		layer->AddEntity(entity);
	} else {
		BLZR_CORE_WARN("Layer with name '{}' does not exist!", layerName);
	}
}

void Scene::RemoveEntityFromLayer(const std::string &layerName,
								  Ref<Entity> entity) {
	Ref<Layer> layer = m_LayerManager->GetLayerByName(layerName);
	if (layer) {
		layer->RemoveEntity(entity);
	} else {
		BLZR_CORE_WARN("Layer with name '{}' does not exist!", layerName);
	}
}

void Scene::BindScene(sol::state &lua) {
	lua.new_usertype<Scene>("Scene", sol::call_constructor,
							sol::factories([]() -> std::shared_ptr<Scene> {
								return std::make_shared<Scene>();
							}),
							"GetLayerManager", &Scene::GetLayerManager,
							"Update", &Scene::Update, "Render", &Scene::Render);
}

} // namespace Blazr
