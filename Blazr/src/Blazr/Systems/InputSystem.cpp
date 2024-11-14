#include "GLFW/glfw3.h"
#include "Blazr/Core/Log.h"
#include "Blazr/Events/KeyEvent.h"
#include "InputSystem.h"
#include "entt.hpp"
#include "sol.hpp"

Ref<Blazr::InputSystem> Blazr::InputSystem::instance = nullptr;

Blazr::InputSystem::InputSystem() {
	// TODO: Add mouse buttons
	m_Buttons = {};
	std::array<int, 127> keys = {GLFW_KEY_SPACE,
								 GLFW_KEY_APOSTROPHE,
								 GLFW_KEY_COMMA,
								 GLFW_KEY_MINUS,
								 GLFW_KEY_PERIOD,
								 GLFW_KEY_SLASH,
								 GLFW_KEY_0,
								 GLFW_KEY_1,
								 GLFW_KEY_2,
								 GLFW_KEY_3,
								 GLFW_KEY_4,
								 GLFW_KEY_5,
								 GLFW_KEY_6,
								 GLFW_KEY_7,
								 GLFW_KEY_8,
								 GLFW_KEY_9,
								 GLFW_KEY_SEMICOLON,
								 GLFW_KEY_EQUAL,
								 GLFW_KEY_A,
								 GLFW_KEY_B,
								 GLFW_KEY_C,
								 GLFW_KEY_D,
								 GLFW_KEY_E,
								 GLFW_KEY_F,
								 GLFW_KEY_G,
								 GLFW_KEY_H,
								 GLFW_KEY_I,
								 GLFW_KEY_J,
								 GLFW_KEY_K,
								 GLFW_KEY_L,
								 GLFW_KEY_M,
								 GLFW_KEY_N,
								 GLFW_KEY_O,
								 GLFW_KEY_P,
								 GLFW_KEY_Q,
								 GLFW_KEY_R,
								 GLFW_KEY_S,
								 GLFW_KEY_T,
								 GLFW_KEY_U,
								 GLFW_KEY_V,
								 GLFW_KEY_W,
								 GLFW_KEY_X,
								 GLFW_KEY_Y,
								 GLFW_KEY_Z,
								 GLFW_KEY_LEFT_BRACKET,
								 GLFW_KEY_BACKSLASH,
								 GLFW_KEY_RIGHT_BRACKET,
								 GLFW_KEY_GRAVE_ACCENT,
								 GLFW_KEY_WORLD_1,
								 GLFW_KEY_WORLD_2,
								 GLFW_KEY_ESCAPE,
								 GLFW_KEY_ENTER,
								 GLFW_KEY_TAB,
								 GLFW_KEY_BACKSPACE,
								 GLFW_KEY_INSERT,
								 GLFW_KEY_DELETE,
								 GLFW_KEY_RIGHT,
								 GLFW_KEY_LEFT,
								 GLFW_KEY_DOWN,
								 GLFW_KEY_UP,
								 GLFW_KEY_PAGE_UP,
								 GLFW_KEY_PAGE_DOWN,
								 GLFW_KEY_HOME,
								 GLFW_KEY_END,
								 GLFW_KEY_CAPS_LOCK,
								 GLFW_KEY_SCROLL_LOCK,
								 GLFW_KEY_NUM_LOCK,
								 GLFW_KEY_PRINT_SCREEN,
								 GLFW_KEY_PAUSE,
								 GLFW_KEY_F1,
								 GLFW_KEY_F2,
								 GLFW_KEY_F3,
								 GLFW_KEY_F4,
								 GLFW_KEY_F5,
								 GLFW_KEY_F6,
								 GLFW_KEY_F7,
								 GLFW_KEY_F8,
								 GLFW_KEY_F9,
								 GLFW_KEY_F10,
								 GLFW_KEY_F11,
								 GLFW_KEY_F12,
								 GLFW_KEY_F13,
								 GLFW_KEY_F14,
								 GLFW_KEY_F15,
								 GLFW_KEY_F16,
								 GLFW_KEY_F17,
								 GLFW_KEY_F18,
								 GLFW_KEY_F19,
								 GLFW_KEY_F20,
								 GLFW_KEY_F21,
								 GLFW_KEY_F22,
								 GLFW_KEY_F23,
								 GLFW_KEY_F24,
								 GLFW_KEY_F25,
								 GLFW_KEY_KP_0,
								 GLFW_KEY_KP_1,
								 GLFW_KEY_KP_2,
								 GLFW_KEY_KP_3,
								 GLFW_KEY_KP_4,
								 GLFW_KEY_KP_5,
								 GLFW_KEY_KP_6,
								 GLFW_KEY_KP_7,
								 GLFW_KEY_KP_8,
								 GLFW_KEY_KP_9,
								 GLFW_KEY_KP_DECIMAL,
								 GLFW_KEY_KP_DIVIDE,
								 GLFW_KEY_KP_MULTIPLY,
								 GLFW_KEY_KP_SUBTRACT,
								 GLFW_KEY_KP_ADD,
								 GLFW_KEY_KP_ENTER,
								 GLFW_KEY_KP_EQUAL,
								 GLFW_KEY_LEFT_SHIFT,
								 GLFW_KEY_LEFT_CONTROL,
								 GLFW_KEY_LEFT_ALT,
								 GLFW_KEY_LEFT_SUPER,
								 GLFW_KEY_RIGHT_SHIFT,
								 GLFW_KEY_RIGHT_CONTROL,
								 GLFW_KEY_RIGHT_ALT,
								 GLFW_KEY_RIGHT_SUPER,
								 GLFW_KEY_MENU};

	// Initialize each key with a default ButtonState
	for (int key : keys) {
		m_Buttons[key] = {false, false, false};
	}
}

void Blazr::InputSystem::RegisterLuaKeybinds(sol::state &lua) {
	lua.set("KEY_SPACE", GLFW_KEY_SPACE);
	lua.set("KEY_APOSTROPHE", GLFW_KEY_APOSTROPHE); /* ' */
	lua.set("KEY_COMMA", GLFW_KEY_COMMA);			/* , */
	lua.set("KEY_MINUS", GLFW_KEY_MINUS);			/* - */
	lua.set("KEY_PERIOD", GLFW_KEY_PERIOD);			/* . */
	lua.set("KEY_SLASH", GLFW_KEY_SLASH);			/* / */
	lua.set("KEY_0", GLFW_KEY_0);
	lua.set("KEY_1", GLFW_KEY_1);
	lua.set("KEY_2", GLFW_KEY_2);
	lua.set("KEY_3", GLFW_KEY_3);
	lua.set("KEY_4", GLFW_KEY_4);
	lua.set("KEY_5", GLFW_KEY_5);
	lua.set("KEY_6", GLFW_KEY_6);
	lua.set("KEY_7", GLFW_KEY_7);
	lua.set("KEY_8", GLFW_KEY_8);
	lua.set("KEY_9", GLFW_KEY_9);
	lua.set("KEY_SEMICOLON", GLFW_KEY_SEMICOLON); /* ; */
	lua.set("KEY_EQUAL", GLFW_KEY_EQUAL);		  /* = */
	lua.set("KEY_A", GLFW_KEY_A);
	lua.set("KEY_B", GLFW_KEY_B);
	lua.set("KEY_C", GLFW_KEY_C);
	lua.set("KEY_D", GLFW_KEY_D);
	lua.set("KEY_E", GLFW_KEY_E);
	lua.set("KEY_F", GLFW_KEY_F);
	lua.set("KEY_G", GLFW_KEY_G);
	lua.set("KEY_H", GLFW_KEY_H);
	lua.set("KEY_I", GLFW_KEY_I);
	lua.set("KEY_J", GLFW_KEY_J);
	lua.set("KEY_K", GLFW_KEY_K);
	lua.set("KEY_L", GLFW_KEY_L);
	lua.set("KEY_M", GLFW_KEY_M);
	lua.set("KEY_N", GLFW_KEY_N);
	lua.set("KEY_O", GLFW_KEY_O);
	lua.set("KEY_P", GLFW_KEY_P);
	lua.set("KEY_Q", GLFW_KEY_Q);
	lua.set("KEY_R", GLFW_KEY_R);
	lua.set("KEY_S", GLFW_KEY_S);
	lua.set("KEY_T", GLFW_KEY_T);
	lua.set("KEY_U", GLFW_KEY_U);
	lua.set("KEY_V", GLFW_KEY_V);
	lua.set("KEY_W", GLFW_KEY_W);
	lua.set("KEY_X", GLFW_KEY_X);
	lua.set("KEY_Y", GLFW_KEY_Y);
	lua.set("KEY_Z", GLFW_KEY_Z);
	lua.set("KEY_LEFT_BRACKET", GLFW_KEY_LEFT_BRACKET);	  /* [ */
	lua.set("KEY_BACKSLASH", GLFW_KEY_BACKSLASH);		  /* \ */
	lua.set("KEY_RIGHT_BRACKET", GLFW_KEY_RIGHT_BRACKET); /* ] */
	lua.set("KEY_GRAVE_ACCENT", GLFW_KEY_GRAVE_ACCENT);	  /* ` */
	lua.set("KEY_WORLD_1", GLFW_KEY_WORLD_1);			  /* non-US #1 */
	lua.set("KEY_WORLD_2", GLFW_KEY_WORLD_2);			  /* non-US #2 */

	/* Function keys */
	lua.set("KEY_ESCAPE", GLFW_KEY_ESCAPE);
	lua.set("KEY_ENTER", GLFW_KEY_ENTER);
	lua.set("KEY_TAB", GLFW_KEY_TAB);
	lua.set("KEY_BACKSPACE", GLFW_KEY_BACKSPACE);
	lua.set("KEY_INSERT", GLFW_KEY_INSERT);
	lua.set("KEY_DELETE", GLFW_KEY_DELETE);
	lua.set("KEY_RIGHT", GLFW_KEY_RIGHT);
	lua.set("KEY_LEFT", GLFW_KEY_LEFT);
	lua.set("KEY_DOWN", GLFW_KEY_DOWN);
	lua.set("KEY_UP", GLFW_KEY_UP);
	lua.set("KEY_PAGE_UP", GLFW_KEY_PAGE_UP);
	lua.set("KEY_PAGE_DOWN", GLFW_KEY_PAGE_DOWN);
	lua.set("KEY_HOME", GLFW_KEY_HOME);
	lua.set("KEY_END", GLFW_KEY_END);
	lua.set("KEY_CAPS_LOCK", GLFW_KEY_CAPS_LOCK);
	lua.set("KEY_SCROLL_LOCK", GLFW_KEY_SCROLL_LOCK);
	lua.set("KEY_NUM_LOCK", GLFW_KEY_NUM_LOCK);
	lua.set("KEY_PRINT_SCREEN", GLFW_KEY_PRINT_SCREEN);
	lua.set("KEY_PAUSE", GLFW_KEY_PAUSE);
	lua.set("KEY_F1", GLFW_KEY_F1);
	lua.set("KEY_F2", GLFW_KEY_F2);
	lua.set("KEY_F3", GLFW_KEY_F3);
	lua.set("KEY_F4", GLFW_KEY_F4);
	lua.set("KEY_F5", GLFW_KEY_F5);
	lua.set("KEY_F6", GLFW_KEY_F6);
	lua.set("KEY_F7", GLFW_KEY_F7);
	lua.set("KEY_F8", GLFW_KEY_F8);
	lua.set("KEY_F9", GLFW_KEY_F9);
	lua.set("KEY_F10", GLFW_KEY_F10);
	lua.set("KEY_F11", GLFW_KEY_F11);
	lua.set("KEY_F12", GLFW_KEY_F12);
	lua.set("KEY_F13", GLFW_KEY_F13);
	lua.set("KEY_F14", GLFW_KEY_F14);
	lua.set("KEY_F15", GLFW_KEY_F15);
	lua.set("KEY_F16", GLFW_KEY_F16);
	lua.set("KEY_F17", GLFW_KEY_F17);
	lua.set("KEY_F18", GLFW_KEY_F18);
	lua.set("KEY_F19", GLFW_KEY_F19);
	lua.set("KEY_F20", GLFW_KEY_F20);
	lua.set("KEY_F21", GLFW_KEY_F21);
	lua.set("KEY_F22", GLFW_KEY_F22);
	lua.set("KEY_F23", GLFW_KEY_F23);
	lua.set("KEY_F24", GLFW_KEY_F24);
	lua.set("KEY_F25", GLFW_KEY_F25);

	/* Keypad */
	lua.set("KEY_KP_0", GLFW_KEY_KP_0);
	lua.set("KEY_KP_1", GLFW_KEY_KP_1);
	lua.set("KEY_KP_2", GLFW_KEY_KP_2);
	lua.set("KEY_KP_3", GLFW_KEY_KP_3);
	lua.set("KEY_KP_4", GLFW_KEY_KP_4);
	lua.set("KEY_KP_5", GLFW_KEY_KP_5);
	lua.set("KEY_KP_6", GLFW_KEY_KP_6);
	lua.set("KEY_KP_7", GLFW_KEY_KP_7);
	lua.set("KEY_KP_8", GLFW_KEY_KP_8);
	lua.set("KEY_KP_9", GLFW_KEY_KP_9);
	lua.set("KEY_KP_DECIMAL", GLFW_KEY_KP_DECIMAL);
	lua.set("KEY_KP_DIVIDE", GLFW_KEY_KP_DIVIDE);
	lua.set("KEY_KP_MULTIPLY", GLFW_KEY_KP_MULTIPLY);
	lua.set("KEY_KP_SUBTRACT", GLFW_KEY_KP_SUBTRACT);
	lua.set("KEY_KP_ADD", GLFW_KEY_KP_ADD);
	lua.set("KEY_KP_ENTER", GLFW_KEY_KP_ENTER);
	lua.set("KEY_KP_EQUAL", GLFW_KEY_KP_EQUAL);

	/* Modifier keys */
	lua.set("KEY_LEFT_SHIFT", GLFW_KEY_LEFT_SHIFT);
	lua.set("KEY_LEFT_CONTROL", GLFW_KEY_LEFT_CONTROL);
	lua.set("KEY_LEFT_ALT", GLFW_KEY_LEFT_ALT);
	lua.set("KEY_LEFT_SUPER", GLFW_KEY_LEFT_SUPER);
	lua.set("KEY_RIGHT_SHIFT", GLFW_KEY_RIGHT_SHIFT);
	lua.set("KEY_RIGHT_CONTROL", GLFW_KEY_RIGHT_CONTROL);
	lua.set("KEY_RIGHT_ALT", GLFW_KEY_RIGHT_ALT);
	lua.set("KEY_RIGHT_SUPER", GLFW_KEY_RIGHT_SUPER);
	lua.set("KEY_MENU", GLFW_KEY_MENU);
}
void Blazr::InputSystem::CreateInputLuaBind(sol::state &lua) {
	RegisterLuaKeybinds(lua);
	Ref<InputSystem> inputSystem = InputSystem::GetInstance();
	lua.new_usertype<InputSystem>(
		"InputSystem", sol::no_constructor, "key_pressed",
		[&](int key) { return inputSystem->IsKeyPressed(key); }, "key_released",
		[&](int key) { return inputSystem->IsKeyReleased(key); },
		"key_repeating",
		[&](int key) { return inputSystem->IsKeyRepeating(key); });
}
void Blazr::InputSystem::Update() {}

bool Blazr::InputSystem::IsKeyPressed(int key) {
	Ref<InputSystem> inputSystem = InputSystem::GetInstance();
	if (inputSystem->m_Buttons.find(key) == inputSystem->m_Buttons.end()) {
		BLZR_CORE_ERROR("Key not found in input system!");
		return false;
	}

	Button &button = inputSystem->m_Buttons[key];
	bool pressed = glfwGetKey(glfwGetCurrentContext(), key) == GLFW_PRESS;
	button.Update(pressed);
	return button.isPressed;
}

bool Blazr::InputSystem::IsKeyReleased(int key) {
	Ref<InputSystem> inputSystem = InputSystem::GetInstance();
	if (inputSystem->m_Buttons.find(key) == inputSystem->m_Buttons.end()) {
		BLZR_CORE_ERROR("Key not found in input system!");
		return false;
	}

	Button &button = inputSystem->m_Buttons[key];
	bool pressed = glfwGetKey(glfwGetCurrentContext(), key) == GLFW_RELEASE;
	button.Update(!pressed);
	return button.isReleased;
}

bool Blazr::InputSystem::IsKeyRepeating(int key) {
	Ref<InputSystem> inputSystem = InputSystem::GetInstance();
	if (inputSystem->m_Buttons.find(key) == inputSystem->m_Buttons.end()) {
		BLZR_CORE_ERROR("Key not found in input system!");
		return false;
	}

	Button &button = inputSystem->m_Buttons[key];
	bool pressed = glfwGetKey(glfwGetCurrentContext(), key) == GLFW_PRESS;
	button.Update(pressed);
	return button.isRepeating;
}
