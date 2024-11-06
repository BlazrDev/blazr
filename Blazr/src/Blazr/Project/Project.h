#include "Blazr.h"
#include <filesystem>

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

	ProjectConfig &GetConfig() { return m_Config; }

	static Ref<Project> GetActive() { return s_ActiveProject; }

	static Ref<Project> New();
	static Ref<Project> Load(const std::filesystem::path &path);
	static bool SaveActive(const std::filesystem::path &path);

  private:
	ProjectConfig m_Config;
	std::filesystem::path m_ProjectDirectory;

	inline static Ref<Project> s_ActiveProject;
};
} // namespace Blazr
