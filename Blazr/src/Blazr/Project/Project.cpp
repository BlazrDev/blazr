#include "blzrpch.h"

#include "Project.h"
#include "ProjectSerializer.h"

namespace Blazr {
Ref<Project> Project::New() {
	s_ActiveProject = CreateRef<Project>();
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
} // namespace Blazr
