#include "blzrpch.h"
#include "AssetManager.h"
#include "Blazr/Core/Log.h"
#include "Blazr/Project/ProjectSerializer.h"
#include "Blazr/Renderer/ShaderLoader.h"
#include "Blazr/Renderer/Texture2D.h"
#include "Blazr/Resources/AssetManager.h"
#include <algorithm>
#include <ranges>
#include <vector>

Ref<Blazr::AssetManager> Blazr::AssetManager::instance = nullptr;
bool Blazr::AssetManager::LoadTexture(const std::string &name,
									  const std::string &texturePath,
									  bool pixelArt, bool tileset) {
	// TODO: pixelArt
	auto texture = std::make_shared<Texture2D>(texturePath, tileset);

	if (!texture) {
		BLZR_CORE_ERROR("Failed to load texture: {0}", texturePath);
		return false;
	}

	m_mapTextures[name] = std::move(texture);
	return true;
}

const Ref<Blazr::Texture2D>
Blazr::AssetManager::GetTexture(const std::string &name) {
	auto textureIterator = m_mapTextures.find(name);
	if (textureIterator == m_mapTextures.end()) {
		// BLZR_CORE_ERROR("Texture not found: {0}", name);
		auto texture = std::make_shared<Texture2D>("assets/white_texture.png");
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

bool Blazr::AssetManager::LoadMusic(const std::string &name,
									const std::string &musicPath,
									const std::string &musicDescription) {
	if (m_mapMusic.find(name) != m_mapMusic.end()) {
		BLZR_CORE_ERROR("Music file already loaded: {0}", name);
		return false;
	}

	Mix_Music *m_music = Mix_LoadMUS(musicPath.c_str());
	if (!m_music) {
		BLZR_CORE_ERROR("Music file not found: {0}", musicPath);
		return false;
	}

	auto music = std::make_shared<Music>(
		SoundProperties{name, musicDescription, musicPath,
						Mix_MusicDuration(m_music)},
		m_music);
	music->SetPath(musicPath);

	m_mapMusic.emplace(name, std::move(music));
	BLZR_CORE_INFO("Music file loaded: {0}", musicPath);
	return true;
}

Ref<Blazr::Music> Blazr::AssetManager::GetMusic(const std::string &name) {
	auto musicIterator = m_mapMusic.find(name);
	if (musicIterator == m_mapMusic.end()) {
		BLZR_CORE_ERROR("Music not found: {0}", name);
		return nullptr;
	}

	return musicIterator->second;
}
std::map<std::string, Ref<Blazr::Music>> &Blazr::AssetManager::getAllMusic() {
	return m_mapMusic;
}

bool Blazr::AssetManager::LoadEffect(const std::string &name,
									 const std::string &effectPath,
									 const std::string &effectDescription) {
	if (m_mapEffect.find(name) != m_mapEffect.end()) {
		BLZR_CORE_ERROR("Effect file already loaded: {0}", name);
		return false;
	}

	Mix_Chunk *m_chunk = Mix_LoadWAV(effectPath.c_str());

	if (!m_chunk) {
		BLZR_CORE_ERROR("Sound effect not found: {0}", effectPath);
		return false;
	}

	int frequency;
	Uint16 format;
	int channels;
	if (!Mix_QuerySpec(&frequency, &format, &channels)) {
		BLZR_CORE_ERROR("Failed to query audio specification: {0}",
						Mix_GetError());
		return false;
	}

	// Calculate the duration of the Mix_Chunk
	Uint32 length = m_chunk->alen;
	int bytesPerSample = (SDL_AUDIO_BITSIZE(format) / 8) * channels;
	Uint32 numSamples = length / bytesPerSample;
	double duration = static_cast<double>(numSamples) / frequency;
	auto effect = std::make_shared<Effect>(
		SoundProperties{name, effectDescription, effectPath, duration},
		m_chunk);
	effect->SetPath(effectPath);

	m_mapEffect.emplace(name, std::move(effect));
	BLZR_CORE_INFO("Sound effect loaded: {0}", effectPath);
	return true;
}

Ref<Blazr::Effect> Blazr::AssetManager::GetEffect(const std::string &name) {
	auto effectIterator = m_mapEffect.find(name);
	if (effectIterator == m_mapEffect.end()) {
		BLZR_CORE_ERROR("Effect not found: {0}", name);
		return nullptr;
	}

	return effectIterator->second;
}
std::map<std::string, Ref<Blazr::Effect>> &
Blazr::AssetManager::getAllEffects() {
	return m_mapEffect;
}
std::map<std::string, Ref<Blazr::Texture2D>> &
Blazr::AssetManager::getAllTextures() {
	return m_mapTextures;
}

std::vector<std::string> Blazr::AssetManager::GetKeysTexturesTileset() {

	std::vector<std::string> keysVector;
	for (const auto &pair : m_mapTextures) {
		if (pair.second->isTileset()) {
			keysVector.push_back(pair.first);
		}
	}
	return keysVector;
}

// void Blazr::AssetManager::CreateLuaEntityBind(sol::state_view &lua) {
// 	lua.new_usertype<AssetManager>(
// 		"AssetManager", sol::constructors<AssetManager()>(),
// 		"get_instance", []() {return AssetManager::GetInstance().get();},
// 		"load_music", &AssetManager::LoadMusic,
// 		"load_effect", &AssetManager::LoadEffect,
// 		"load_shader", &AssetManager::LoadShader,
// 		"load_texture", &AssetManager::LoadTexture

// 	);
// }

void Blazr::AssetManager::CreateLuaAssetManager(sol::state &lua,
												Registry &registry) {
	auto &asset_manager = registry.GetContext<std::shared_ptr<AssetManager>>();
	if (!asset_manager) {
		BLZR_CORE_ERROR(
			"Failed to bind Asset Manager to Lua - Does not exist in registry");
		return;
	}

	lua.new_usertype<AssetManager>(
		"AssetManager", sol::no_constructor, "load_music",
		[&](const std::string &name, const std::string &path,
			const std::string &desc) {
			return asset_manager->LoadMusic(name, path, desc);
		},
		"load_effect",
		[&](const std::string &name, const std::string &path,
			const std::string &desc) {
			return asset_manager->LoadEffect(name, path, desc);
		},
		"load_shader",
		[&](const std::string &name, const std::string &vertexPath,
			const std::string &fragmentPath) {
			return asset_manager->LoadShader(name, vertexPath, fragmentPath);
		},
		"load_texture",
		[&](const std::string &name, const std::string &texturePath,
			bool pixelArt, bool tileset) {
			return asset_manager->LoadTexture(name, texturePath, pixelArt,
											  tileset);
		});
}

bool Blazr::AssetManager::LoadScene(const std::string &scenePath,
									Ref<sol::state> luaState) {
	if (Ref<Scene> scene =
			ProjectSerializer::DeserializeScene(scenePath, luaState)) {
		Project::GetActive()->AddScene(scene->GetName(), scene);
		return true;
	}
	return false;
}

void Blazr::AssetManager::to_json(nlohmann::json &j,
								  Ref<AssetManager> assetManager) {
	j["Shaders"] = nlohmann::json::object();
	for (const auto &[name, shader] : assetManager->m_mapShaders) {
		if (shader) {
			j["Shaders"][name] = {{"vertexPath", shader->GetVertexPath()},
								  {"fragmentPath", shader->GetFragmentPath()}};
		}
	}

	j["Textures"] = nlohmann::json::object();
	for (const auto &[name, texture] : assetManager->m_mapTextures) {
		if (texture) {
			j["Textures"][name] = {{"path", texture->GetPath()},
								   {"pixelArt", true}};
		}
	}

	j["Music"] = nlohmann::json::object();
	for (const auto &[name, music] : assetManager->m_mapMusic) {
		if (music) {
			nlohmann::json propertiesJson;
			SoundProperties::to_json(propertiesJson, music->GetProperties());
			j["Music"][name] = {
				{"path", music->GetPath()},
				{"properties", propertiesJson},
				{"description",
				 music->GetProperties().description}}; // Add description
		}
	}

	j["Effects"] = nlohmann::json::object();
	for (const auto &[name, effect] : assetManager->m_mapEffect) {
		if (effect) {
			nlohmann::json propertiesJson;
			SoundProperties::to_json(propertiesJson, effect->GetProperties());
			j["Effects"][name] = {
				{"path", effect->GetPath()},
				{"properties", propertiesJson},
				{"channel", effect->getChannel()},
				{"description",
				 effect->GetProperties().description}}; // Add description
		}
	}
}

void Blazr::AssetManager::from_json(const nlohmann::json &j,
									Ref<AssetManager> assetManager) {
	assetManager->m_mapShaders.clear();
	assetManager->m_mapTextures.clear();
	assetManager->m_mapMusic.clear();
	assetManager->m_mapEffect.clear();

	std::string projectDir = Project::GetProjectDirectory().string();

	if (j.contains("Shaders")) {
		for (const auto &[name, shaderJson] : j.at("Shaders").items()) {
			std::string vertexPath =
				projectDir + "/" +
				shaderJson.at("vertexPath").get<std::string>();
			std::string fragmentPath =
				projectDir + "/" +
				shaderJson.at("fragmentPath").get<std::string>();
			assetManager->LoadShader(name, vertexPath, fragmentPath);
		}
	}

	if (j.contains("Textures")) {
		for (const auto &[name, textureJson] : j.at("Textures").items()) {
			std::string path =
				projectDir + "/" + textureJson.at("path").get<std::string>();
			bool pixelArt = textureJson.at("pixelArt").get<bool>();
			assetManager->LoadTexture(name, path, pixelArt);
		}
	}

	if (j.contains("Music")) {
		for (const auto &[name, musicJson] : j.at("Music").items()) {
			std::string path =
				projectDir + "/" + musicJson.at("path").get<std::string>();
			std::string description =
				musicJson.at("description").get<std::string>();
			assetManager->LoadMusic(name, path, description);
		}
	}

	if (j.contains("Effects")) {
		for (const auto &[name, effectJson] : j.at("Effects").items()) {
			std::string path =
				projectDir + "/" + effectJson.at("path").get<std::string>();
			std::string description =
				effectJson.at("description").get<std::string>();
			int channel = effectJson.at("channel").get<int>();
			assetManager->LoadEffect(name, path, description);
		}
	}
}

void Blazr::AssetManager::Reset() {
	instance = std::make_shared<AssetManager>();
	LoadTexture("default", "assets/white_texture.png");
	LoadTexture("collider", "assets/collider.png");
}
