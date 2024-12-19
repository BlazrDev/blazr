#include "blzrpch.h"
#include "Blazr/Core/Log.h"
#include "Blazr/Ecs/Components/AnimationComponent.h"
#include "Blazr/Ecs/Components/BoxColliderComponent.h"
#include "Blazr/Ecs/Components/Identification.h"
#include "Blazr/Ecs/Components/PhysicsComponent.h"
#include "Blazr/Ecs/Components/ScriptComponent.h"
#include "Blazr/Ecs/Components/SpriteComponent.h"
#include "Blazr/Ecs/Components/TileComponent.h"
#include "Blazr/Ecs/Components/TransformComponent.h"
#include "Layer.h"

namespace Blazr {
bool Layer::showColliders = false;

void Layer::AddEntity(Ref<Entity> entity) { entities.push_back(entity); }
void Layer::RemoveEntity(Ref<Entity> entity) {
	entities.erase(std::remove(entities.begin(), entities.end(), entity),
				   entities.end());
}

void Layer::RemoveEntity(const std::string &name) {
	for (auto it = entities.begin(); it != entities.end();) {
		if (*it == nullptr) {
			BLZR_CORE_WARN("Encountered a null entity in layer!");
			it = entities.erase(it); // Remove the null entity
			continue;
		}

		if ((*it)->GetComponent<Identification>().name == name) {
			it = entities.erase(it); // Erase and update iterator
		} else {
			++it;
		}
	}
}

void Layer::Render(Registry &registry) {
	for (auto entity : entities) {
		if (entity->HasComponent<TransformComponent>() &&
			entity->HasComponent<SpriteComponent>()) {
			auto &transform = entity->GetComponent<TransformComponent>();
			auto &sprite = entity->GetComponent<SpriteComponent>();

			auto entityHandler = entity->GetEntityHandler();

			sprite.generateTextureCoordinates();

			if (entity->GetGroup() == "grid") {
				Blazr::Renderer2D::DrawQuad(
					entityHandler, {transform.position.x, transform.position.y},
					{sprite.width, sprite.height}, sprite.color);
			}

			if (entity->GetName() != "Collider" &&
				entity->GetGroup() != "grid") {
				Renderer2D::DrawQuad(registry, entityHandler);
			}

			if (showColliders) {
				if (entity->HasComponent<BoxColliderComponent>()) {
					auto &collider =
						entity->GetComponent<BoxColliderComponent>();
					Renderer2D::DrawQuad(
						entt::null,
						{transform.position.x + collider.offset.x,
						 transform.position.y + collider.offset.y},
						{collider.width * transform.scale.x,
						 collider.height * transform.scale.y},
						{255, 0, 0, 0.4});
				}
			}
		}
	}
}

void Layer::BindLayer(sol::state &lua) {
	lua.new_usertype<Layer>("Layer", "name", &Layer::name, "zIndex",
							&Layer::zIndex);
}

void Layer::to_json(nlohmann::json &j, const Ref<Layer> layer) {
	j["zIndex"] = layer->zIndex;
	j["Entities"] = nlohmann::json::array();
	std::for_each(layer->entities.begin(), layer->entities.end(),
				  [&](Ref<Entity> entity) {
					  // if (entity->HasComponent<TileComponent>()) {
					  //  return;
					  // }

					  nlohmann::json entityJson;
					  if (entity->HasComponent<AnimationComponent>()) {
						  AnimationComponent::to_json(
							  entityJson["Animation"],
							  entity->GetComponent<AnimationComponent>());
					  }
					  if (entity->HasComponent<SpriteComponent>()) {
						  SpriteComponent::to_json(
							  entityJson["Sprite"],
							  entity->GetComponent<SpriteComponent>());
					  }
					  if (entity->HasComponent<TransformComponent>()) {
						  TransformComponent::to_json(
							  entityJson["Transform"],
							  entity->GetComponent<TransformComponent>());
					  }
					  if (entity->HasComponent<BoxColliderComponent>()) {
						  BoxColliderComponent::to_json(
							  entityJson["BoxCollider"],
							  entity->GetComponent<BoxColliderComponent>());
					  }
					  if (entity->HasComponent<ScriptComponent>()) {
						  ScriptComponent::to_json(
							  entityJson["Script"],
							  entity->GetComponent<ScriptComponent>());
					  }
					  if (entity->HasComponent<PhysicsComponent>()) {
						  PhysicsComponent::to_json(
							  entityJson["Physics"],
							  entity->GetComponent<PhysicsComponent>());
					  }
					  if (entity->HasComponent<Identification>()) {
						  Identification::to_json(
							  entityJson["Identification"],
							  entity->GetComponent<Identification>());
					  }
					  j["Entities"].push_back(entityJson);
				  });
}

void Layer::from_json(const nlohmann::json &j, Ref<Layer> layer) {
	layer->entities.clear();
	if (j.contains("zIndex")) {
		layer->zIndex = j.at("zIndex");
	}

	if (j.contains("Entities")) {
		for (const auto &entityJson : j.at("Entities")) {

			Ref<Entity> entity;
			if (entityJson.contains("Identification")) {
				Identification identification;
				Identification::from_json(entityJson.at("Identification"),
										  identification);
				entity = CreateRef<Entity>(*Registry::GetInstance(),
										   identification.name,
										   identification.group);
			} else {
				BLZR_CORE_ERROR(
					"Error deserializing entity. No identification found.");
				return;
			}

			if (entityJson.contains("Animation")) {
				AnimationComponent animation;
				AnimationComponent::from_json(entityJson.at("Animation"),
											  animation);
				entity->AddComponent<AnimationComponent>(animation);
			}
			if (entityJson.contains("Sprite")) {
				SpriteComponent sprite;
				SpriteComponent::from_json(entityJson.at("Sprite"), sprite);
				entity->AddComponent<SpriteComponent>(sprite);
			}
			if (entityJson.contains("Transform")) {
				TransformComponent transform;
				TransformComponent::from_json(entityJson.at("Transform"),
											  transform);
				entity->AddComponent<TransformComponent>(transform);
			}
			if (entityJson.contains("BoxCollider")) {
				BoxColliderComponent boxCollider;
				BoxColliderComponent::from_json(entityJson.at("BoxCollider"),
												boxCollider);
				entity->AddComponent<BoxColliderComponent>(boxCollider);
			}
			if (entityJson.contains("Script")) {
				ScriptComponent script;
				ScriptComponent::from_json(entityJson.at("Script"), script);
				entity->AddComponent<ScriptComponent>(script);
			}
			if (entityJson.contains("Physics")) {
				PhysicsComponent physics;
				PhysicsComponent::from_json(entityJson.at("Physics"), physics);
				entity->AddComponent<PhysicsComponent>(physics);
			}

			layer->entities.push_back(entity);
		}
	}
}
} // namespace Blazr
