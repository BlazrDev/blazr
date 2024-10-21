#include "sol.hpp"
#include <string>
namespace Blazr {
struct ScriptComponent {
	sol::protected_function update(sol::lua_state &lua) {
		return lua["update"];
	}
};
} // namespace Blazr
