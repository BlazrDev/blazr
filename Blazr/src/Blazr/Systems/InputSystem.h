#pragma once

#include "sol.hpp"

namespace Blazr {
class InputSystem {
  public:
	InputSystem() = default;
	~InputSystem() = default;

	void Update();
	static void RegisterLuaKeybinds(sol::state &lua);
	static void CreateInputLuaBind(sol::state &lua);

	static bool IsKeyPressed(int key);

  private:
};
}; // namespace Blazr
