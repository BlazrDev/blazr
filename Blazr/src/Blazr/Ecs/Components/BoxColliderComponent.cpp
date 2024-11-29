#include "blzrpch.h"
#include "BoxColliderComponent.h"

void Blazr::BoxColliderComponent::CreateLuaBoxColliderComponentBind(
	sol::state &lua) {
	lua.new_usertype<BoxColliderComponent>(
		"BoxColliderComponent", "type_id",
		&entt::type_hash<BoxColliderComponent>::value, sol::call_constructor,
		sol::factories(
			[](int width, int height, glm::vec2 offset) {
				return BoxColliderComponent{.width = width,
											.height = height,
											.offset = offset,
											.colliding = false};
			},
			[](glm::vec2 size, glm::vec2 offset) {
				return BoxColliderComponent{.width = static_cast<int>(size.x),
											.height = static_cast<int>(size.y),
											.offset = offset,
											.colliding = false};
			}),
		"width", &BoxColliderComponent::width, "height",
		&BoxColliderComponent::height, "offset", &BoxColliderComponent::offset,
		"colliding", &BoxColliderComponent::colliding);
}
