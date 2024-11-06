
#include "blzrpch.h"
#include "ProjectSerializer.h"
#include <fstream>

namespace Blazr {
using json = nlohmann::json;

json ProjectConfigToJson(const ProjectConfig &config) {
	return json{{"name", config.name},
				{"StartScene", config.StartScene.string()},
				{"AssetDirectory", config.AssetDirectory.string()},
				{"ScriptPath", config.ScriptPath.string()}};
}

ProjectConfig ProjectConfigFromJson(const json &j) {
	ProjectConfig config;
	config.name = j.at("name").get<std::string>();
	config.StartScene = j.at("StartScene").get<std::string>();
	config.AssetDirectory = j.at("AssetDirectory").get<std::string>();
	config.ScriptPath = j.at("ScriptPath").get<std::string>();
	return config;
}

bool ProjectSerializer::Serialize(const Ref<Project> &project,
								  const std::filesystem::path &filepath) {
	if (!project) {
		BLZR_CORE_ERROR("Cannot serialize a null project!");
		return false;
	}

	json j;
	j["ProjectDirectory"] = project->GetProjectDirectory().string();
	j["Config"] = ProjectConfigToJson(project->GetConfig());

	std::ofstream ofs(filepath);
	if (!ofs) {
		BLZR_CORE_ERROR("Failed to open file for serialization: {}",
						filepath.string());
		return false;
	}
	ofs << j.dump(4); // Pretty-print JSON with indentation
	return true;
}

Ref<Project>
ProjectSerializer::Deserialize(const std::filesystem::path &filepath) {
	std::ifstream ifs(filepath);
	if (!ifs) {
		BLZR_CORE_ERROR("Failed to open file for deserialization: {}",
						filepath.string());
		return nullptr;
	}

	json j;
	ifs >> j;

	auto project = CreateRef<Project>();
	project->GetProjectDirectory() =
		j.at("ProjectDirectory").get<std::string>();
	project->GetConfig() = ProjectConfigFromJson(j.at("Config"));

	Project::GetActive() = project;
	return project;
}

} // namespace Blazr
