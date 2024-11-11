#include "GLFW/glfw3.h"
#include "InputSystem.h"
#include "entt.hpp"
#include "sol.hpp"

void Blazr::InputSystem::RegisterLuaKeybinds(sol::state &lua) {
	lua.set("KEY_A", GLFW_KEY_A);
}

void Blazr::InputSystem::CreateInputLuaBind(sol::state &lua) {
	RegisterLuaKeybinds(lua);
	lua.new_usertype<InputSystem>("InputSystem", sol::no_constructor,
								  "key_pressed",
								  [&](int key) { return IsKeyPressed(key); });
}
void Blazr::InputSystem::Update() {}

bool Blazr::InputSystem::IsKeyPressed(int key) {
	return glfwGetKey(glfwGetCurrentContext(), key) == GLFW_PRESS;
}
