#pragma once
#include <json.hpp>
#include <sol.hpp>
#include <string>

namespace Blazr {
struct ScriptComponent {
	sol::protected_function update{sol::lua_nil};
	sol::protected_function render{sol::lua_nil};

	std::string scriptPath;

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
