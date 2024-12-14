#pragma once
#include "blzrpch.h"
#include "Blazr/Core/Log.h"
#include "sol.hpp"
#include <entt.hpp>

namespace Blazr {
struct TileComponent {
	std::string name = "";
	static void CreateLuaTileComponentBind(sol::state &lua) {
		lua.new_usertype<TileComponent>(
			"TileComponent", "type_id", &entt::type_hash<TileComponent>::value,
			sol::call_constructor, sol::factories([](const std::string &name) {
				return TileComponent{.name = name};
			}),
			"name", &TileComponent::name);
	};
};
} // namespace Blazr
