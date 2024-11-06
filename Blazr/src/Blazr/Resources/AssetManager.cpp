#include "Blazr/Core/Log.h"
#include "Blazr/Renderer/ShaderLoader.h"
#include "Blazr/Renderer/Texture2D.h"
#include "Blazr/Resources/AssetManager.h"

Ref<Blazr::AssetManager> Blazr::AssetManager::instance = nullptr;
bool Blazr::AssetManager::LoadTexture(const std::string &name,
									  const std::string &texturePath,
									  bool pixelArt) {
	if (m_mapTextures.find(name) != m_mapTextures.end()) {
		BLZR_CORE_ERROR("Texture already loaded: {0}", name);
		return false;
	}

	// TODO: pixelArt
	auto texture = std::make_shared<Texture2D>(texturePath);

	if (!texture) {
		BLZR_CORE_ERROR("Failed to load texture: {0}", texturePath);
		return false;
	}

	m_mapTextures.emplace(name, std::move(texture));
	return true;
}
const Ref<Blazr::Texture2D>
Blazr::AssetManager::GetTexture(const std::string &name) {
	auto textureIterator = m_mapTextures.find(name);
	if (textureIterator == m_mapTextures.end()) {
		BLZR_CORE_ERROR("Texture not found: {0}", name);
		return nullptr;
	}

	return textureIterator->second;
}

bool Blazr::AssetManager::LoadShader(const std::string &name,
									 const std::string &vertexPath,
									 const std::string &fragmentPath) {
	if (m_mapShaders.find(name) != m_mapShaders.end()) {
		BLZR_CORE_ERROR("Shader already loaded: {0}", name);
		return false;
	}

	auto shader = std::move(ShaderLoader::Create(vertexPath, fragmentPath));

	if (!shader) {
		BLZR_CORE_ERROR("Failed to load shader: {0}", name);
		return false;
	}

	m_mapShaders.emplace(name, std::move(shader));
	return true;
}
Ref<Blazr::Shader> Blazr::AssetManager::GetShader(const std::string &name) {
	auto shaderIterator = m_mapShaders.find(name);
	if (shaderIterator == m_mapShaders.end()) {
		BLZR_CORE_ERROR("Texture not found: {0}", name);
		return nullptr;
	}

	return shaderIterator->second;
}
