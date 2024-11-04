#pragma once
#include "Blazr/Ecs/Registry.h"
#include "Blazr/Renderer/RendererAPI.h"
#include "sol.hpp"
#include <glm.hpp>
#include <string>

namespace Blazr {
struct Object {
	float x = 0.0f;
	float y = 0.0f;
	float width = 0.0f;
	float height = 0.0f;
};
struct SpriteComponent {
	float width = 0.0f;
	float height = 0.0f;

	Object object{.x = 0.0f, .y = 0.0f, .width = 0.0f, .height = 0.0f};
	glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};

	int startX = -1;
	int startY = -1;

	void generateObject(int textureWidth, int textureHeight) {
		object.width = width / textureWidth;
		object.height = height / textureHeight;
		object.x = startX * object.width;
		object.y = startY * object.height;
	}
	std::string texturePath;

	static void CreateLuaSpriteComponentBind(sol::state &lua,
											 Registry &registry);
};
} // namespace Blazr
