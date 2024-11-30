#pragma once
#include "Blazr/Core/Log.h"
#include "Blazr/Scene/Scene.h"
#include <filesystem>
#include <unordered_map>

namespace Blazr {
struct ProjectConfig {
	std::string name = "Untitled";
	std::filesystem::path StartScene;
	std::filesystem::path AssetDirectory;
	std::filesystem::path ScriptPath;
};
class Project {
  public:
	static const std::filesystem::path &GetProjectDirectory() {
		if (s_ActiveProject != nullptr) {
			return s_ActiveProject->m_ProjectDirectory;
		} else {
			BLZR_CORE_ERROR("No active project!");
			return std::filesystem::path("");
		}
	}

	static std::filesystem::path GetAssetDirectory() {
		return GetProjectDirectory() / s_ActiveProject->m_Config.AssetDirectory;
	}

	static std::filesystem::path
	GetAssetFileSystemPath(const std::filesystem::path &path) {
		return GetAssetDirectory() / path;
	}

	ProjectConfig &GetConfig() { return m_Config; }

	static Ref<Project> GetActive() { return s_ActiveProject; }
	static void SetActive(Ref<Project> project) { s_ActiveProject = project; }

	static Ref<Project> New();
	static Ref<Project> Load(const std::filesystem::path &path);
	static bool SaveActive(const std::filesystem::path &path);

	void SetProjectDirectory(const std::filesystem::path &directory) {
		m_ProjectDirectory = directory;
	}

	const std::unordered_map<std::string, Ref<Scene>> &GetLoadedScenes() const {
		return m_LoadedScenes;
	}

	void AddScene(const std::string &name, const Ref<Scene> &scene) {
		m_LoadedScenes[name] = scene;
	}

	void RemoveScene(const std::string &name) { m_LoadedScenes.erase(name); }

	Ref<Scene> GetScene(const std::string &name) const {
		auto it = m_LoadedScenes.find(name);
		if (it != m_LoadedScenes.end()) {
			return it->second;
		}
		return nullptr;
	}

	std::filesystem::path GetSceneFilePath(const std::string &name) const {
		return GetProjectDirectory() / "Scenes" / (name + ".bsc");
	}

  private:
	std::unordered_map<std::string, Ref<Scene>> m_LoadedScenes;
	ProjectConfig m_Config;
	std::filesystem::path m_ProjectDirectory;

	inline static Ref<Project> s_ActiveProject;
};
} // namespace Blazr
