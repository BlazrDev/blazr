#pragma once
#include "blzrpch.h"
#include "Blazr/Ecs/Entity.h"

namespace Blazr {
class BoxColliderSystem {
  public:
	BoxColliderSystem() = default;
	~BoxColliderSystem() = default;

	bool isColliding(Entity &entity1, Entity &entity2);
	static void CreateLuaBoxColliderSystemBind(sol::state &lua);
};
} // namespace Blazr
