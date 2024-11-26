#pragma once
#include "Blazr/Core/Log.h"
#include "Blazr/Ecs/Registry.h"
#include "sol.hpp"
#include <glm.hpp>
#include <json.hpp>
#include <string>

namespace Blazr {
struct Object {
	int coordX = 0;
	int coordY = 0;
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

	glm::vec2 textureCoordinates[4] = {
		{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f}};

	// Method to generate texture coordinates based on the frame position
	void generateObject(int textureWidth, int textureHeight) {
		// Calculate fractional frame size based on the texture sheet dimensions

		// Set the position of the current frame within the texture
		object.width = width / textureWidth;
		object.height = height / textureHeight;
		// BLZR_CORE_INFO("Object width: {0}, Object height: {1}, textureWidth:
		// "
		// 			   "{2}, textureHeight:{3}",
		// 			   object.width, object.height, textureWidth,
		// 			   textureHeight);

		object.x = startX * object.width;
		object.y = startY * object.height;

		// Set the texture coordinates for the sprite
		textureCoordinates[0] = {object.coordX * object.width,
								 (object.coordY + 1) * object.height};

		textureCoordinates[1] = {(object.coordX + 1) * object.width,
								 (object.coordY + 1) * object.height};
		textureCoordinates[2] = {(object.coordX + 1) * object.width,
								 object.coordY * object.height};
		textureCoordinates[3] = {object.coordX * object.width,
								 object.coordY * object.height};
	}

	void generateTextureCoordinates() {
		textureCoordinates[0] = {object.coordX * object.width,
								 (object.coordY + 1) * object.height};

		textureCoordinates[1] = {(object.coordX + 1) * object.width,
								 (object.coordY + 1) * object.height};
		textureCoordinates[2] = {(object.coordX + 1) * object.width,
								 object.coordY * object.height};
		textureCoordinates[3] = {object.coordX * object.width,
								 object.coordY * object.height};
	}
	std::string texturePath;

	static void CreateLuaSpriteComponentBind(sol::state &lua,
											 Registry &registry);
	static void from_json(const nlohmann::json &j, SpriteComponent &sprite);
	static void to_json(nlohmann::json &j, const SpriteComponent &sprite);
};

} // namespace Blazr
