#pragma once
#include <json.hpp>
// #include <string>

namespace Blazr {
struct SoundProperties {
	std::string name{""}, description{""}, filename{""};
	double duration{0.0};
	static void to_json(nlohmann::json &j,
						const SoundProperties &soundProperties) {
		j = nlohmann::json{{"name", soundProperties.name},
						   {"description", soundProperties.description},
						   {"filename", soundProperties.filename},
						   {"duration", soundProperties.duration}};
	}
	static void from_json(const nlohmann::json &j,
						  SoundProperties &soundProperties) {
		if (j.contains("name")) {
			soundProperties.name = j.at("name").get<std::string>();
		}
		if (j.contains("description")) {
			soundProperties.description =
				j.at("description").get<std::string>();
		}
		if (j.contains("filename")) {
			soundProperties.filename = j.at("filename").get<std::string>();
		}
		if (j.contains("duration")) {
			soundProperties.duration = j.at("duration").get<double>();
		}
	}
};
} // namespace Blazr
