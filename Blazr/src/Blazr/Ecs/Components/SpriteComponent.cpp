#include "blzrpch.h"
#include "Blazr/Core/Log.h"
#include "Blazr/Resources/AssetManager.h"
#include "SpriteComponent.h"
#include <memory>

void Blazr::SpriteComponent::CreateLuaSpriteComponentBind(sol::state &lua,
														  Registry &registry) {
	lua.new_usertype<SpriteComponent>(
		"SpriteComponent", "type_id", &entt::type_hash<SpriteComponent>::value,
		sol::call_constructor,
		sol::factories([](float width, float height,
						  const std::string &texturePath, int start_x,
						  int start_y, int layer) {
			return SpriteComponent{
				.width = width,
				.height = height,
				.object = Object{},
				.startX = start_x,
				.startY = start_y,
				.texturePath = texturePath
				// .layer = layer;
			};
		}),
		"texture_path", &SpriteComponent::texturePath, "width",
		&SpriteComponent::width, "height", &SpriteComponent::height, "start_x",
		&SpriteComponent::startX, "start_y", &SpriteComponent::startY,
		// "layer", &SpriteComponent::layer
		"generate_object", [&](SpriteComponent &sprite) {
			auto &assetManager = AssetManager::GetInstance();
			auto texture = assetManager->GetTexture(sprite.texturePath);

			if (!texture) {
				BLZR_CORE_ERROR("Texture not found: {0}", sprite.texturePath);
				return;
			}

			sprite.generateObject(texture->GetWidth(), texture->GetHeight());
		});
}

void Blazr::SpriteComponent::from_json(const nlohmann::json &j,
									   SpriteComponent &sprite) {
	// Deserialize basic fields
	sprite.width = j.at("width").get<float>();
	sprite.height = j.at("height").get<float>();

	// Deserialize Object struct
	const auto &objectJson = j.at("object");
	sprite.object.coordX = objectJson.at("coordX").get<int>();
	sprite.object.coordY = objectJson.at("coordY").get<int>();
	sprite.object.x = objectJson.at("x").get<float>();
	sprite.object.y = objectJson.at("y").get<float>();
	sprite.object.width = objectJson.at("width").get<float>();
	sprite.object.height = objectJson.at("height").get<float>();

	// Deserialize color
	auto color = j.at("color").get<std::vector<float>>();
	sprite.color = {color[0], color[1], color[2], color[3]};

	// Deserialize texture coordinates
	const auto &textureCoordsJson = j.at("textureCoordinates");
	for (size_t i = 0; i < textureCoordsJson.size(); ++i) {
		sprite.textureCoordinates[i] = {textureCoordsJson[i][0],
										textureCoordsJson[i][1]};
	}

	// Deserialize start positions and texture path
	sprite.startX = j.at("startX").get<int>();
	sprite.startY = j.at("startY").get<int>();
	sprite.texturePath = j.at("texturePath").get<std::string>();
}

void Blazr::SpriteComponent::to_json(nlohmann::json &j,
									 const SpriteComponent &sprite) {
	// Serialize Object struct
	nlohmann::json objectJson = {
		{"coordX", sprite.object.coordX}, {"coordY", sprite.object.coordY},
		{"x", sprite.object.x},			  {"y", sprite.object.y},
		{"width", sprite.object.width},	  {"height", sprite.object.height}};

	// Serialize texture coordinates
	nlohmann::json textureCoordsJson = nlohmann::json::array();
	for (const auto &coord : sprite.textureCoordinates) {
		textureCoordsJson.push_back({coord.x, coord.y});
	}

	j = nlohmann::json{
		{"width", sprite.width},
		{"height", sprite.height},
		{"object", objectJson},
		{"color",
		 {sprite.color.r, sprite.color.g, sprite.color.b, sprite.color.a}},
		{"startX", sprite.startX},
		{"startY", sprite.startY},
		{"textureCoordinates", textureCoordsJson},
		{"texturePath", sprite.texturePath}};
}
