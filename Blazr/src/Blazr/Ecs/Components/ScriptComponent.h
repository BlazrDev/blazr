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

	void LoadScript(sol::state &luaState, Ref<Entity> entity,
					const std::string &name) {
		try {
			luaState[name] = sol::lua_nil;

			BLZR_CORE_ERROR("LOADING SCRIPT: {0}", scriptPath);
			sol::table script = luaState.require_file("script", scriptPath);

			Entity &entRef = *entity;
			luaState[name] = entRef;

			if (script["on_update"].valid()) {
				auto &scriptComponent =
					entRef.GetComponent<Blazr::ScriptComponent>();
				scriptComponent.update = script["on_update"];
			} else {
				BLZR_CORE_WARN("Script does not contain 'on_update' function.");
			}

			if (script["on_render"].valid()) {
				auto &scriptComponent =
					entRef.GetComponent<Blazr::ScriptComponent>();
				scriptComponent.render = script["on_render"];
			} else {
				BLZR_CORE_WARN("Script does not contain 'on_render' function.");
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
