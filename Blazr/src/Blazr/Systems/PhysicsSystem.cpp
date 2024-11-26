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

	auto scaledWidth = 1280.0f / METERS_TO_PIXELS;
	auto scaledHeight = 720.0f / METERS_TO_PIXELS;

	for (auto entity : boxView) {
		auto &physics = boxView.get<PhysicsComponent>(entity);
		auto &transform = boxView.get<TransformComponent>(entity);
		auto &collider = boxView.get<BoxColliderComponent>(entity);

		auto rigidBody = physics.GetRigidBody();

		const auto &pos = b2Body_GetPosition(rigidBody);

		transform.position.x =
			((scaledWidth / 2.f) + pos.x) * METERS_TO_PIXELS -
			(collider.width * transform.scale.x) / 2.f - collider.offset.x;

		transform.position.y =
			((scaledHeight / 2.f) - pos.y) * METERS_TO_PIXELS -
			(collider.height * transform.scale.y) / 2.f - collider.offset.y;

		if (!b2Body_IsFixedRotation(rigidBody)) {
			transform.rotation =
				glm::degrees(b2Rot_GetAngle(b2Body_GetRotation(rigidBody)));
		}
	}
}
