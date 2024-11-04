#pragma once
#include <map>
#include <memory>
#include <string>

#include "Blazr/Renderer/Shader.h"
#include "Blazr/Renderer/Texture2D.h"

namespace Blazr {
class AssetManager {
  private:
	std::map<std::string, std::shared_ptr<Shader>> m_mapShaders;
	std::map<std::string, std::shared_ptr<Texture2D>> m_mapTextures;

  public:
	AssetManager() = default;
	~AssetManager() = default;

	bool LoadTexture(const std::string &name, const std::string &texturePath,
					 bool pixelArt = true);
	const Texture2D &GetTexture(const std::string &name);

	bool LoadShader(const std::string &name, const std::string &vertexPath,
					const std::string &fragmentPath);
	Shader &GetShader(const std::string &name);
};
} // namespace Blazr
