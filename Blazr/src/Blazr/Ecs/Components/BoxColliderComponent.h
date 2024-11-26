#include "entt.hpp"
#include "glm.hpp"
#include "sol.hpp"
#include <json.hpp>

namespace Blazr {
struct BoxColliderComponent {
	int width{0}, height{0};
	glm::vec2 offset{0.0f, 0.0f};
	bool colliding{false};

	static void CreateLuaBoxColliderComponentBind(sol::state &lua);
	static void from_json(const nlohmann::json &j,
						  BoxColliderComponent &box_collider);
	static void to_json(nlohmann::json &j,
						const BoxColliderComponent &box_collider);
};

} // namespace Blazr
