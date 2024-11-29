#pragma once
#include "sol.hpp"
#include <glm.hpp>
#include <json.hpp>

namespace Blazr {
struct TransformComponent {
	glm::vec2 position = {0.0f, 0.0f};
	glm::vec2 scale = {1.0f, 1.0f};
	float rotation = 0.0f;

	static void CreateLuaTransformComponentBind(sol::state &lua);
	static void to_json(nlohmann::json &j, const TransformComponent &transform);
	static void from_json(const nlohmann::json &j,
						  TransformComponent &transform);
};

} // namespace Blazr
