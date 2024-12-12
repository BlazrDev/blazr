#pragma once
#include "Blazr/Core/Log.h"
#include "Blazr/Ecs/Entity.h"
#include <json.hpp>
#include <sol.hpp>
#include <string>

namespace Blazr {
struct ScriptComponent {
	sol::protected_function update{sol::lua_nil};
	sol::protected_function render{sol::lua_nil};

	std::string scriptPath;
	void LoadScript(sol::state &luaState, Entity entity) {
		try {
			// Load the Lua script file
			sol::table script = luaState.require_file("script", scriptPath);

			// Bind the entity to Lua
			luaState["entity"] = entity;

			// Check for 'on_update' function
			if (script["on_update"].valid()) {
				update = script["on_update"];
			} else {
				BLZR_CORE_WARN(
					"Script at {} does not contain 'on_update' function.",
					scriptPath);
			}

			// Check for 'on_render' function
			if (script["on_render"].valid()) {
				render = script["on_render"];
			} else {
				BLZR_CORE_WARN(
					"Script at {} does not contain 'on_render' function.",
					scriptPath);
			}
		} catch (const sol::error &e) {
			BLZR_CORE_ERROR("Failed to load script at {}: {}", scriptPath,
							e.what());
		}
	}
	static void to_json(nlohmann::json &j,
						const ScriptComponent &scriptComponent) {
		j = nlohmann::json{{"scriptPath", scriptComponent.scriptPath}};
	}
	static void from_json(const nlohmann::json &j,
						  ScriptComponent &scriptComponent) {
		scriptComponent.scriptPath = j.at("scriptPath").get<std::string>();
	}
};
} // namespace Blazr
