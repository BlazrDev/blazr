#include "Blazr/Ecs/Components/BoxColliderComponent.h"
#include "Blazr/Ecs/Entity.h"
#include "BoxColliderSystem.h"
#include "sol.hpp"

bool Blazr::BoxColliderSystem::isColliding(Entity &entity1, Entity &entity2) {
	if (!entity1.HasComponent<BoxColliderComponent>() ||
		!entity2.HasComponent<BoxColliderComponent>()) {
		return false;
	}

	if (entity1.GetEntityHandler() == entity2.GetEntityHandler()) {
		return false;
	}

	if (entity1.GetGroup() == entity2.GetGroup()) {
		return false;
	}

	auto &collider1 = entity1.GetComponent<BoxColliderComponent>();
	auto &collider2 = entity2.GetComponent<BoxColliderComponent>();

	auto &transform1 = entity1.GetComponent<TransformComponent>();
	auto &transform2 = entity2.GetComponent<TransformComponent>();

	// Calculate the edges of collider1
	float left1 = transform1.position.x + collider1.offset.x;
	float right1 = left1 + collider1.width;
	float top1 = transform1.position.y + collider1.offset.y;
	float bottom1 = top1 + collider1.height;

	BLZR_CORE_INFO(
		"Collider1: left1: {0}, right1: {1}, top1: {2}, bottom1: {3}", left1,
		right1, top1, bottom1);

	BLZR_CORE_INFO(
		"Collider1: x: {0}, y: {1}, colliderWidth: {2}, colliderHeight: {3}, "
		"colliderOffsetX: {4}, colliderOffsetY: {5}",
		transform1.position.x, transform1.position.y, collider1.width,
		collider1.height, collider1.offset.x, collider1.offset.y);

	// Calculate the edges of collider2
	float left2 = transform2.position.x + collider2.offset.x;
	float right2 = left2 + collider2.width;
	float top2 = transform2.position.y + collider2.offset.y;
	float bottom2 = top2 + collider2.height;

	BLZR_CORE_INFO(
		"Collider2: left2: {0}, right2: {1}, top2: {2}, bottom2: {3}", left2,
		right2, top2, bottom2);

	// Check for overlap between the two colliders
	bool isColliding = (left1 < right2 && right1 > left2 && top1 < bottom2 &&
						bottom1 > top2) &&
					   collider1.colliding && collider2.colliding;

	// // Update the colliding state
	// collider1.colliding = isColliding;
	// collider2.colliding = isColliding;

	return isColliding;
}

void Blazr::BoxColliderSystem::CreateLuaBoxColliderSystemBind(sol::state &lua) {
	BoxColliderSystem boxColliderSystem{};
	lua.new_usertype<BoxColliderSystem>(
		"BoxColliderSystem", sol::no_constructor, "is_colliding",
		/* &BoxColliderSystem::isColliding */
		[&](Entity &e1, Entity &e2) {
			return boxColliderSystem.isColliding(e1, e2);
		});
}
