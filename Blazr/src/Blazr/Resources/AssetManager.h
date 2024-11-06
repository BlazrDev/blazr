#pragma once
#include <map>
#include <memory>
#include <string>

#include "Blazr/Renderer/Shader.h"
#include "Blazr/Renderer/Texture2D.h"

namespace Blazr {
class AssetManager {
  private:
	std::map<std::string, Ref<Shader>> m_mapShaders;
	std::map<std::string, Ref<Texture2D>> m_mapTextures;

  public:
	AssetManager() = default;
	~AssetManager() = default;

	static Ref<AssetManager> &GetInstance() {
		if (instance == nullptr) {
			instance = std::make_shared<AssetManager>();
		}
		return instance;
	}

	bool LoadTexture(const std::string &name, const std::string &texturePath,
					 bool pixelArt = true);
	const Ref<Texture2D> GetTexture(const std::string &name);

	bool LoadShader(const std::string &name, const std::string &vertexPath,
					const std::string &fragmentPath);
	Ref<Blazr::Shader> GetShader(const std::string &name);

  private:
	static Ref<AssetManager> instance;
};
} // namespace Blazr
