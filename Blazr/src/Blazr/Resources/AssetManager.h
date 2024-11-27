#pragma once
#include <map>
#include <memory>
#include <string>

#include "Blazr/Renderer/Shader.h"
#include "Blazr/Renderer/Texture2D.h"
#include "Blazr/Systems/Sounds/Music.h"
#include "Blazr/Systems/Sounds/SoundProperties.h"
#include "Blazr/Systems/Sounds/Effect.h"
#include "Blazr/Ecs/Registry.h"
#include "sol.hpp"

namespace Blazr {
class AssetManager {
  private:
	std::map<std::string, Ref<Shader>> m_mapShaders;
	std::map<std::string, Ref<Texture2D>> m_mapTextures;

	std::map<std::string, Ref<Music>> m_mapMusic;
	std::map<std::string, Ref<Effect>> m_mapEffect;

  public:
	AssetManager() = default;
	~AssetManager() = default;

	BLZR_API static Ref<AssetManager> &GetInstance() {
		if (instance == nullptr) {
			instance = std::make_shared<AssetManager>();
		}
		return instance;
	}
	// static void CreateLuaEntityBind(sol::state_view &lua);
	static void CreateLuaAssetManager(sol::state &lua, Registry &registry);

	bool LoadTexture(const std::string &name, const std::string &texturePath,
					 bool pixelArt = true);
	const Ref<Texture2D> GetTexture(const std::string &name);

	bool LoadShader(const std::string &name, const std::string &vertexPath,
					const std::string &fragmentPath);
	Ref<Blazr::Shader> GetShader(const std::string &name);

	bool LoadMusic(const std::string &name, const std::string &musicPath,
				   const std::string &musicDescription);
	Ref<Blazr::Music> GetMusic(const std::string &name);

	bool LoadEffect(const std::string &name, const std::string &effectPath,
					const std::string &effectDescription);
	Ref<Blazr::Effect> GetEffect(const std::string &name);

  private:
	static Ref<AssetManager> instance;
};
}; // namespace Blazr
