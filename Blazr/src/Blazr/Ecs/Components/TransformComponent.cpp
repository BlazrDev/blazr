#include "blzrpch.h"
#include "TransformComponent.h"
#include "entt.hpp"
#include "ext/vector_float2.hpp"
#include "sol.hpp"
void Blazr::TransformComponent::CreateLuaTransformComponentBind(
	sol::state &lua) {
	lua.new_usertype<TransformComponent>(
		"TransformComponent", "type_id",
		&entt::type_hash<TransformComponent>::value, sol::call_constructor,
		sol::factories(
			[](const glm::vec2 &position, const glm::vec2 &scale,
			   float rotation) {
				return TransformComponent{
					.position = position, .scale = scale, .rotation = rotation};
			},
			[](float x, float y, float scale_x, float scale_y, float rotation) {
				return TransformComponent{.position = glm::vec2{x, y},
										  .scale = glm::vec2{scale_x, scale_y},
										  .rotation = rotation};
			}),
		"position",
		[](TransformComponent &transform) {
			return std::make_tuple(transform.position.x, transform.position.y);
		},
		"scale",
		[](TransformComponent &transform) {
			return std::make_tuple(transform.scale.x, transform.scale.y);
		},
		"rotation", &TransformComponent::rotation, "set_position",
		[](TransformComponent &transform, float x, float y) {
			transform.position = glm::vec2(x, y);
		},
		"set_scale",
		[](TransformComponent &transform, float x, float y) {
			transform.scale = glm::vec2(x, y);
		}

	);
}
