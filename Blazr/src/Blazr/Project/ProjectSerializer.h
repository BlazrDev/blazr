#pragma once

#include "Project.h"
#include <filesystem>
#include <json.hpp>

namespace Blazr {

class ProjectSerializer {
  public:
	static bool BLZR_API Serialize(const Ref<Project> &project,
								   const std::filesystem::path &filepath);

	static Ref<Project> Deserialize(const std::filesystem::path &filepath,
									Ref<sol::state> luaState);
	static bool SerializeScene(const Ref<Scene> &scene,
							   const std::filesystem::path &filepath);
	static Ref<Scene> DeserializeScene(const std::filesystem::path &filepath,
									   Ref<sol::state> luaState);
};

} // namespace Blazr
