#include "Blazr/Core/Log.h"
#include "Blazr/Ecs/Components/BoxColliderComponent.h"
#include "Blazr/Ecs/Components/PhysicsComponent.h"
#include "Blazr/Ecs/Components/TransformComponent.h"
#include "PhysicsSystem.h"
#include "box2d/box2d.h"

Blazr::PhysicsSystem::PhysicsSystem(Registry &registry)
	: m_Registry(registry) {}
void Blazr::PhysicsSystem::Update(Registry &registry) {
	auto boxView =
		registry.GetRegistry()
			.view<PhysicsComponent, TransformComponent, BoxColliderComponent>();

	auto scaledWidth = 1280.0f;
	auto scaledHeight = 720.0f;

	for (auto entity : boxView) {
		auto &physics = boxView.get<PhysicsComponent>(entity);
		auto rigidBody = physics.GetRigidBody();

		if (!rigidBody) {
			BLZR_CORE_ERROR("PhysicsSystem::Update: RigidBody is nullptr");
			return;
		}

		auto &transform = boxView.get<TransformComponent>(entity);
		auto &collider = boxView.get<BoxColliderComponent>(entity);

		const auto &pos = rigidBody->GetPosition();

		BLZR_CORE_INFO("PhysicsSystem::Update postition: {0}, {1}", pos.x,
					   pos.y);

		// transform.position.x =
		// 	((scaledWidth / 2.f) + pos.x) * METERS_TO_PIXELS -
		// 	(collider.width * transform.scale.x) / 2.f;
		//
		// transform.position.y =
		// 	((scaledHeight / 2.f) + pos.y) * METERS_TO_PIXELS -
		// 	(collider.height * transform.scale.y) / 2.f;

		transform.position.x =
			(scaledWidth * 0.5f - collider.width * transform.scale.x * 0.5f) +
			pos.x * METERS_TO_PIXELS;

		transform.position.y =
			(scaledHeight * 0.5f - collider.height * transform.scale.y * 0.5f) +
			pos.y * METERS_TO_PIXELS;

		BLZR_CORE_INFO(
			"PhysicsSystem::Update: {0}, {1}",
			(scaledWidth * 0.5f - collider.width * transform.scale.x * 0.5f) +
				pos.x * METERS_TO_PIXELS,

			(scaledHeight * 0.5f - collider.height * transform.scale.y * 0.5f) +
				pos.y * METERS_TO_PIXELS);

		if (!rigidBody->IsFixedRotation()) {
			transform.rotation = glm::degrees(rigidBody->GetAngle());
		}
	}
}
