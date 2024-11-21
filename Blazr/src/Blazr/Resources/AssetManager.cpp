#include "blzrpch.h"
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

bool Blazr::AssetManager::LoadMusic(const std::string &name, const std::string &musicPath, const std::string &musicDescription) {
	if(m_mapMusic.find(name) != m_mapMusic.end()) {
		BLZR_CORE_ERROR("Music file already loaded: {0}", name);
		return false;
	}

	Mix_Music* m_music = Mix_LoadMUS(musicPath.c_str());
	if(!m_music) {
		BLZR_CORE_ERROR("Music file not found: {0}", musicPath);
		return false;
	}

	auto music = std::make_shared<Music>(SoundProperties {
		name, 
		musicDescription, 
		musicPath, 
		Mix_MusicDuration(m_music)
	}, m_music);
			
	m_mapMusic.emplace(name, std::move(music));
	BLZR_CORE_INFO("Music file loaded: {0}", musicPath);
	return true;
}

Ref<Blazr::Music> Blazr::AssetManager::GetMusic(const std::string &name) {
	auto musicIterator = m_mapMusic.find(name);
	if(musicIterator == m_mapMusic.end()) {
		BLZR_CORE_ERROR("Music not found: {0}", name);
		return nullptr;
	}

	return musicIterator->second;
}

bool Blazr::AssetManager::LoadEffect(const std::string &name, const std::string &effectPath, const std::string &effectDescription) {
	if(m_mapEffect.find(name) != m_mapEffect.end()) {
		BLZR_CORE_ERROR("Effect file already loaded: {0}", name);
		return false;
	}

	Mix_Chunk* m_chunk = Mix_LoadWAV(effectPath.c_str());

	if(!m_chunk) {
		BLZR_CORE_ERROR("Sound effect not found: {0}", effectPath);
		return false;
	}

	int frequency;
    Uint16 format;
    int channels;
    if (!Mix_QuerySpec(&frequency, &format, &channels)) {
        BLZR_CORE_ERROR("Failed to query audio specification: {0}", Mix_GetError());
        return false;
    }

    // Calculate the duration of the Mix_Chunk
    Uint32 length = m_chunk->alen;
    int bytesPerSample = (SDL_AUDIO_BITSIZE(format) / 8) * channels;
    Uint32 numSamples = length / bytesPerSample;
    double duration = static_cast<double>(numSamples) / frequency;
	auto effect = std::make_shared<Effect>(SoundProperties {
		name,
		effectDescription,
		effectPath,
		duration
	}, m_chunk);

	m_mapEffect.emplace(name, std::move(effect));
	BLZR_CORE_INFO("Sound effect loaded: {0}", effectPath);
	return true;

}

Ref<Blazr::Effect> Blazr::AssetManager::GetEffect(const std::string &name) {
	auto effectIterator = m_mapEffect.find(name);
	if(effectIterator ==m_mapEffect.end()) {
		BLZR_CORE_ERROR("Effect not found: {0}", name);
		return nullptr;
	}

	return effectIterator->second;
}
