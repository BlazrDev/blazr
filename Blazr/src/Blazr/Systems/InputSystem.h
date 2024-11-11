#pragma once

#include "GLFW/glfw3.h"
#include "Blazr/Core/Core.h"
#include "Blazr/Core/Log.h"
#include "sol.hpp"
#include <unordered_map>

namespace Blazr {

struct Button {
	bool isPressed;
	bool isReleased;
	bool isRepeating;
	bool wasPressed;

	void Update(bool pressed) {
		using namespace std::chrono;
		// Only set `isPressed` to true on the initial press (when transitioning
		// from not pressed to pressed)
		isPressed = !wasPressed && pressed;
		// Set `isRepeating` if the key remains pressed after the initial press
		// BLZR_CORE_INFO("isPressed: {0}, wasPressed: {1}, pressed: {2}",
		// 			   isPressed, wasPressed, pressed);
		isRepeating = wasPressed && pressed;

		// Set `isReleased` if the key was pressed in the last frame and is now
		// released
		isReleased = wasPressed && !pressed;

		// Update `wasPressed` to the current state for the next update
		wasPressed = pressed;
	}
};
class InputSystem {
  public:
	InputSystem();
	~InputSystem() = default;

	static Ref<InputSystem> &GetInstance() {
		if (instance == nullptr) {
			instance = std::make_shared<InputSystem>();
		}
		return instance;
	}

	void Update();
	static void RegisterLuaKeybinds(sol::state &lua);
	static void CreateInputLuaBind(sol::state &lua);

	bool IsKeyPressed(int key);
	bool IsKeyReleased(int key);
	bool IsKeyRepeating(int key);

  private:
	std::unordered_map<int, Button> m_Buttons;
	static Ref<InputSystem> instance;
};
}; // namespace Blazr
