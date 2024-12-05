#pragma once
#include <json.hpp>
#include <string>

namespace Blazr {
struct Identification {
	std::string name = "GameObject";
	std::string group = "";
	int32_t id = -1;

	static void to_json(nlohmann::json &j,
						const Identification &identification) {
		j = nlohmann::json{{"name", identification.name},
						   {"group", identification.group},
						   {"id", identification.id}};
	}

	static void from_json(const nlohmann::json &j,
						  Identification &identification) {
		identification.name = j.at("name").get<std::string>();

		identification.group = j.at("group").get<std::string>();

		identification.id = j.at("id").get<int32_t>();
	}
};
} // namespace Blazr
