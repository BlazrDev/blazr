#pragma once
#include "../Ecs/Registry.h"
#include "sol.hpp"

namespace Blazr {
class ScriptingSystem {
  private:
	Registry &m_Registry;
	bool m_bMainLoaded;

  public:
	ScriptingSystem(Registry &registry);
	~ScriptingSystem() = default;

	bool LoadMainScript(sol::state &lua);
	void Update();
	void Render();

	static void RegisterLuaBindings(sol::state &lua, Registry &registry);
};
} // namespace Blazr
