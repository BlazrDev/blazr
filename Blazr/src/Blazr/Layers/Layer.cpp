#include "blzrpch.h"
#include "Blazr/Core/Log.h"
#include "Blazr/Ecs/Components/BoxColliderComponent.h"
#include "Layer.h"

namespace Blazr {
void Layer::AddEntity(Ref<Entity> entity) { entities.push_back(entity); }
void Layer::RemoveEntity(Ref<Entity> entity) {
	entities.erase(std::remove(entities.begin(), entities.end(), entity),
				   entities.end());
}
void Layer::Render(Registry &registry) {
	for (auto entity : entities) {
		if (entity->HasComponent<TransformComponent>() &&
			entity->HasComponent<SpriteComponent>()) {
			auto &transform = entity->GetComponent<TransformComponent>();
			auto &sprite = entity->GetComponent<SpriteComponent>();

			auto entityHandler = entity->GetEntityHandler();

			sprite.generateTextureCoordinates();

			Renderer2D::DrawQuad(registry, entityHandler);
			if (entity->HasComponent<BoxColliderComponent>()) {
				auto &collider = entity->GetComponent<BoxColliderComponent>();
				Renderer2D::DrawQuad(entt::null,
									 {transform.position.x + collider.offset.x,
									  transform.position.y + collider.offset.y},
									 {collider.width * transform.scale.x,
									  collider.height * transform.scale.y},
									 {255, 0, 0, 0.6});
			}
		}
	}
}

void Layer::BindLayer(sol::state &lua) {
	lua.new_usertype<Layer>("Layer", "name", &Layer::name, "zIndex",
							&Layer::zIndex);
}
} // namespace Blazr
