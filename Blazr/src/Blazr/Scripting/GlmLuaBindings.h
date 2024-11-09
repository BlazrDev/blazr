#pragma once
#include "sol.hpp"

namespace Blazr {
struct GLMBindings {
	static void CreateLuaGLMBinding(sol::state &lua);
};
} // namespace Blazr
