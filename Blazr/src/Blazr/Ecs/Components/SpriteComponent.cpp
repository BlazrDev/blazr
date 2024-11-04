#include "Blazr/Core/Log.h"
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
				.texturePath = texturePath,
				.object = Object{},
				.startX = start_x,
				.startY = start_y
				// .layer = layer;
			};
		}),
		"texture_path", &SpriteComponent::texturePath, "width",
		&SpriteComponent::width, "height", &SpriteComponent::height, "start_x",
		&SpriteComponent::startX, "start_y", &SpriteComponent::startY,
		// "layer", &SpriteComponent::layer
		"generate_object", [&](SpriteComponent &sprite) {
			// TODO: napraviti assetManager
			auto &assetManager =
				registry.GetContext<std::shared_ptr<AssetManager>>();
			auto texture = assetManager->GetTexture(sprite.texturePath);

			if (texture.getID() == 0) {
				BLZR_CORE_ERROR("Texture not found: {0}", sprite.texturePath);
				return;
			}

			sprite.generateObject(texture.GetWidth(), texture.GetHeight());
		});
}
