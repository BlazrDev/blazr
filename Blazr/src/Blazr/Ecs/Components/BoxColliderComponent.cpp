#include "blzrpch.h"
#include "Blazr/Core/Log.h"
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

void Blazr::BoxColliderComponent::to_json(
	nlohmann::json &j, const Blazr::BoxColliderComponent &box_collider) {
	j = nlohmann::json{
		{"width", box_collider.width},
		{"height", box_collider.height},
		{"offset", {box_collider.offset.x, box_collider.offset.y}},
		{"colliding", box_collider.colliding}};
}

void Blazr::BoxColliderComponent::from_json(
	const nlohmann::json &j, BoxColliderComponent &box_collider) {
	box_collider.width = j.at("width").get<int>();
	box_collider.height = j.at("height").get<int>();

	auto offset = j.at("offset").get<std::vector<float>>();
	if (offset.size() == 2) {
		box_collider.offset = glm::vec2{offset[0], offset[1]};
	} else {
		BLZR_CORE_WARN("Invalid offset array size for BoxColliderComponent");
		box_collider.offset = glm::vec2{0.0f, 0.0f};
	}
	box_collider.colliding = j.at("colliding").get<bool>();
}
