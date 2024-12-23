#pragma once
#include "Blazr/Core/Log.h"
#include "Blazr/Scene/Scene.h"
#include <filesystem>
#include <unordered_map>

namespace Blazr {
struct ProjectConfig {
	std::string name = "Untitled";
	std::string StartSceneName;

	// Useless for now, will later be used for dynamic asset directory
	// management
	std::filesystem::path AssetDirectory;
	std::filesystem::path ScriptDirectory;
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

	static BLZR_API Ref<Project> GetActive() { return s_ActiveProject; }
	static BLZR_API void SetActive(Ref<Project> project) {
		s_ActiveProject = project;
	}

	static BLZR_API Ref<Project> New(const std::filesystem::path &path,
									 const std::string &name = "Untitled");
	static BLZR_API Ref<Project> Load(const std::filesystem::path &path,
									  Ref<sol::state> luaState);
	static BLZR_API bool SaveActive(const std::filesystem::path &path =
										s_ActiveProject->GetProjectDirectory());

	void BLZR_API SetProjectDirectory(const std::filesystem::path &directory) {
		m_ProjectDirectory = directory;
	}

	const BLZR_API std::unordered_map<std::string, Ref<Scene>> &
	GetScenes() const {
		return m_Scenes;
	}

	void BLZR_API AddScene(const std::string &name, const Ref<Scene> &scene) {
		m_Scenes[name] = scene;
	}

	void BLZR_API RemoveScene(const std::string &name) { m_Scenes.erase(name); }

	Ref<Scene> BLZR_API GetScene(const std::string &name) const {
		auto it = m_Scenes.find(name);
		if (it != m_Scenes.end()) {
			return it->second;
		}
		return nullptr;
	}

	std::filesystem::path BLZR_API
	GetSceneFilePath(const std::string &name) const {
		return GetProjectDirectory() / "Scenes" / (name + ".bsc");
	}
	bool BLZR_API RenameScene(const std::string &oldName,
							  const std::string &newName);

	static BLZR_API Ref<Project> s_ActiveProject;

	void BLZR_API BuildProject(const std::string &path);

  private:
	std::unordered_map<std::string, Ref<Scene>> m_Scenes;
	ProjectConfig m_Config;
	std::filesystem::path m_ProjectDirectory;
};
} // namespace Blazr
