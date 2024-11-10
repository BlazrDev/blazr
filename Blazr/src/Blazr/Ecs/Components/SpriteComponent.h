#pragma once
#include "Blazr/Core/Log.h"
#include "Blazr/Ecs/Registry.h"
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

	glm::vec2 textureCoords[4] = {
		{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f}};
	// glm::vec2 textureCoords[4] = {
	// 	{0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}};

	// Method to generate texture coordinates based on the frame position
	void generateObject(int textureWidth, int textureHeight) {
		// Calculate fractional frame size based on the texture sheet dimensions

		// Set the position of the current frame within the texture
		object.width = width / textureWidth;
		object.height = height / textureHeight;

		object.x = startX * object.width;
		object.y = startY * object.height;

		// Calculate texture coordinates for the current frame
		// textureCoords[0] = {object.x, object.y + object.height};
		// textureCoords[1] = {object.x + object.width, object.y +
		// object.height}; textureCoords[2] = {object.x + object.width,
		// object.y}; textureCoords[3] = {object.x, object.y};
	}
	std::string texturePath;

	static void CreateLuaSpriteComponentBind(sol::state &lua,
											 Registry &registry);
};
} // namespace Blazr
