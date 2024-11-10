#pragma once
#include <sol.hpp>

namespace Bazr {
struct ScriptComponent {
	sol::protected_function update{sol::lua_nil}, render{sol::lua_nil};
};
} // namespace Bazr
