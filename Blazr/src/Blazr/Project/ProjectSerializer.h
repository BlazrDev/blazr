#pragma once

#include "Blazr.h"
#include "Project.h"
#include <filesystem>
#include <json.hpp>

namespace Blazr {

class ProjectSerializer {
  public:
	static bool Serialize(const Ref<Project> &project,
						  const std::filesystem::path &filepath);

	static Ref<Project> Deserialize(const std::filesystem::path &filepath);
};

} // namespace Blazr
