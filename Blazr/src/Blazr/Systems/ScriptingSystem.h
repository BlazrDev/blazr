#pragma once
#include "../Ecs/Registry.h"
#include "sol.hpp"

namespace Blazr {
class ScriptingSystem {
  private:
	Registry &m_Registry;
	bool m_bMainLoaded;

  public:
	BLZR_API ScriptingSystem(Registry &registry);
	~ScriptingSystem() = default;

	bool LoadMainScript(sol::state &lua);
	void Update();
	void Render();

	static void RegisterLuaBindings(sol::state &lua, Registry &registry);

	static void RegisterLuaFunctions(sol::state &lua);
};
} // namespace Blazr
