#include "blzrpch.h"
#include "entt.hpp"
#include "glm.hpp"
#include "sol.hpp"

namespace Blazr {
struct BoxColliderComponent {
	int width{0}, height{0};
	glm::vec2 offset{0.0f, 0.0f};
	bool colliding{false};

	static void CreateLuaBoxColliderComponentBind(sol::state &lua);
};

} // namespace Blazr
