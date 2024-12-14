
#include "blzrpch.h"
#include "Blazr/Resources/AssetManager.h"
#include "ProjectSerializer.h"
#include <fstream>

namespace Blazr {
using json = nlohmann::json;

json ProjectConfigToJson(const ProjectConfig &config) {
	return json{{"name", config.name},
				{"StartScene", config.StartSceneName},
				{"AssetDirectory", config.AssetDirectory.string()},
				{"ScriptPath", config.ScriptDirectory.string()}};
}

ProjectConfig ProjectConfigFromJson(const json &j) {
	ProjectConfig config;
	config.name = j.at("name").get<std::string>();
	config.StartSceneName = j.at("StartScene").get<std::string>();
	config.AssetDirectory = j.at("AssetDirectory").get<std::string>();
	config.ScriptDirectory = j.at("ScriptPath").get<std::string>();
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
	nlohmann::json assetJson;
	AssetManager::to_json(assetJson, AssetManager::GetInstance());
	j["AssetManager"] = assetJson;

	std::filesystem::create_directories(filepath.parent_path());
	std::ofstream ofs(
		const_cast<std::filesystem::path &>(filepath).replace_extension(
			".blzrproj"));
	if (!ofs) {
		BLZR_CORE_ERROR("Failed to save project: {}", filepath.string());
		return false;
	}
	ofs << j.dump(4);

	for (const auto &pair : project->GetScenes()) {
		std::string sceneName = pair.first;
		sceneName.resize(std::distance(
			sceneName.begin(),
			std::remove_if(sceneName.begin(), sceneName.end(),
						   [](char c) { return c == ' ' || c == '\''; })));
		if (!SerializeScene(pair.second, pair.first)) {
			BLZR_CORE_ERROR(
				"Unable to save scene {0}. Aborting saving process!",
				pair.first);
		}
	}

	return true;
}

Ref<Project>
ProjectSerializer::Deserialize(const std::filesystem::path &filepath) {
	if (!std::filesystem::exists(filepath)) {
		BLZR_CORE_ERROR("File does not exist: {}", filepath.string());
		return nullptr;
	}

	std::ifstream ifs(filepath);
	if (!ifs) {
		BLZR_CORE_ERROR("Failed to open file for deserialization: {}",
						filepath.string());
		return nullptr;
	}

	json j;
	try {
		ifs >> j;
	} catch (const std::exception &e) {
		BLZR_CORE_ERROR("Error parsing JSON from file: {} - {}",
						filepath.string(), e.what());
		return nullptr;
	}

	auto project = CreateRef<Project>();

	try {

		if (j.contains("ProjectDirectory")) {
			project->SetProjectDirectory(
				j.at("ProjectDirectory").get<std::string>());
		} else {
			BLZR_CORE_WARN("Missing 'ProjectDirectory' in the JSON file.");
		}

		if (j.contains("Config")) {
			project->GetConfig() = ProjectConfigFromJson(j.at("Config"));
		} else {
			BLZR_CORE_WARN("Missing 'Config' in the JSON file.");
		}

		Project::SetActive(project);

		if (j.contains("AssetManager")) {
			AssetManager::from_json(j.at("AssetManager"),
									AssetManager::GetInstance());
		} else {
			BLZR_CORE_WARN("Missing 'AssetManager' in the JSON file.");
		}

		auto scenesPath =
			std::filesystem::path(project->GetProjectDirectory()) / "scenes";

		if (std::filesystem::exists(scenesPath) &&
			std::filesystem::is_directory(scenesPath)) {
			for (const auto &entry :
				 std::filesystem::directory_iterator(scenesPath)) {
				if (entry.is_regular_file() &&
					entry.path().extension() == ".blzrscn") {
					auto scene = DeserializeScene(entry.path());
					if (scene) {
						project->AddScene(scene->GetName(), scene);
						BLZR_CORE_ERROR("Deserialized {0}", scene->GetName());
					} else {
						BLZR_CORE_WARN("FAILED TO DESERIALIZE SCENE:{}",
									   entry.path().string());
					}
				}
			}
		}

	} catch (const std::exception &e) {
		BLZR_CORE_ERROR("Failed to deserialize project: {} - {}",
						filepath.string(), e.what());
		return nullptr;
	}

	return project;
}

bool ProjectSerializer::SerializeScene(const Ref<Scene> &scene,
									   const std::filesystem::path &filepath) {
	if (!scene) {
		BLZR_CORE_ERROR("Cannot serialize a null scene!");
		return false;
	}

	json j;
	scene->Serialize(j);

	std::filesystem::create_directories(
		Project::GetActive()->GetProjectDirectory() / "scenes");
	std::ofstream ofs(
		Project::GetActive()->GetProjectDirectory() / "scenes" /
		const_cast<std::filesystem::path &>(filepath).replace_extension(
			".blzrscn"));
	if (!ofs) {
		BLZR_CORE_ERROR("Failed to open scene file for serialization: {}",
						filepath.string());
		return false;
	}

	ofs << j.dump(4);
	return true;
}

Ref<Scene>
ProjectSerializer::DeserializeScene(const std::filesystem::path &filepath) {
	std::ifstream ifs(filepath);
	if (!ifs) {
		BLZR_CORE_ERROR("Failed to open scene file for deserialization: {}",
						filepath.string());
		return nullptr;
	}

	json j;
	ifs >> j;

	auto scene = CreateRef<Scene>();
	scene->Deserialize(j);

	return scene;
}

} // namespace Blazr
