#include "blzrpch.h"
#include "Blazr/"
#include "Blazr/Renderer/Renderer2D.h"
#include "Blazr/Systems/ScriptingSystem.h"
#include "Scene.h"

namespace Blazr {

Scene::Scene() : m_Camera(1280, 720) {
	m_Camera.SetScale(1.0f);
	m_Camera.SetPosition({0.0f, 0.0f});
}

Scene::~Scene() {}

entt::entity Scene::AddEntity() { return m_Registry.CreateEntity(); }

void Scene::RemoveEntity(entt::entity entity) {

	m_Registry.GetRegistry().destroy(entity);
}

void Scene::Update() {

	if (auto animationSystem =
			m_Registry.GetContext<std::shared_ptr<AnimationSystem>>()) {
		animationSystem->Update();
	}

	if (auto scriptingSystem =
			m_Registry.GetContext<std::shared_ptr<ScriptingSystem>>()) {
		scriptingSystem->Update();
	}
}

void Scene::Render() {

	Renderer2D::BeginScene(m_Camera);

	auto view =
		m_Registry.GetRegistry().view<TransformComponent, SpriteComponent>();
	for (auto entity : view) {
		auto &transform = view.get<TransformComponent>(entity);
		auto &sprite = view.get<SpriteComponent>(entity);

		sprite.generateTextureCoordinates();

		Renderer2D::DrawQuad(m_Registry, entity);
	}

	Renderer2D::EndScene();
	Renderer2D::Flush();
}

} // namespace Blazr
