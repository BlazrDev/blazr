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

	// Calculate the edges of collider2
	float left2 = transform2.position.x + collider2.offset.x;
	float right2 = left2 + collider2.width;
	float top2 = transform2.position.y + collider2.offset.y;
	float bottom2 = top2 + collider2.height;

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
