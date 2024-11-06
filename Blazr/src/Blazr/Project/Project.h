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

  private:
	ProjectConfig m_Config;
	std::filesystem::path m_ProjectDirectory;

	inline static Ref<Project> s_ActiveProject;
};
} // namespace Blazr
