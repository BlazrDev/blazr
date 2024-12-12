#include "blzrpch.h"

#include "Project.h"
#include "ProjectSerializer.h"

namespace Blazr {

Ref<Project> Project::s_ActiveProject = nullptr;

Ref<Project> Project::New(const std::filesystem::path &path,
						  const std::string &name) {
	s_ActiveProject = CreateRef<Project>();
	s_ActiveProject->GetConfig().name = name;
	s_ActiveProject->SetProjectDirectory(path);
	return s_ActiveProject;
}

Ref<Project> Project::Load(const std::filesystem::path &path) {
	Ref<Project> project;

	if ((project = ProjectSerializer::Deserialize(path))) {
		project->m_ProjectDirectory = path.parent_path();
		s_ActiveProject = project;
		return s_ActiveProject;
	}

	return nullptr;
}

bool Project::SaveActive(const std::filesystem::path &path) {
	if (ProjectSerializer::Serialize(s_ActiveProject, path)) {
		s_ActiveProject->m_ProjectDirectory = path.parent_path();
		return true;
	}
	return false;
}

bool Project::RenameScene(const std::string &oldName,
						  const std::string &newName) {
	auto sceneIt = m_Scenes.find(oldName);
	if (sceneIt == m_Scenes.end()) {
		BLZR_CORE_ERROR("Scene with name '{}' not found!", oldName);
		return false;
	}

	if (m_Scenes.find(newName) != m_Scenes.end()) {
		BLZR_CORE_ERROR("Scene with name '{}' already exists!", newName);
		return false;
	}

	Ref<Scene> scene = sceneIt->second;
	m_Scenes.erase(sceneIt);
	m_Scenes[newName] = scene;

	std::filesystem::path oldFilePath = GetSceneFilePath(oldName);
	std::filesystem::path newFilePath = GetSceneFilePath(newName);

	try {
		if (std::filesystem::exists(oldFilePath)) {
			std::filesystem::rename(oldFilePath, newFilePath);
		}
	} catch (const std::filesystem::filesystem_error &e) {
		BLZR_CORE_ERROR("Failed to rename scene file: {}", e.what());
		return false;
	}

	BLZR_CORE_INFO("Scene '{}' renamed to '{}'", oldName, newName);
	return true;
}
} // namespace Blazr
