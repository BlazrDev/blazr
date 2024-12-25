#pragma once

#include "Blazr/Core/Core.h"
#include "Blazr/Core/Log.h"
#include "Blazr/Ecs/Registry.h"
#include "Music.h"
#include "entt.hpp"
#include "sol.hpp"
namespace Blazr {

class SoundPlayer {

  private:
	static Ref<SoundPlayer> instance;
	static int NUM_OF_CHANNELS;
	std::unordered_map<int, bool> channelMuted;

	Ref<Music> currentMusic;

  public:
	std::unordered_map<int, int> channelVolumes;
	BLZR_API static Ref<SoundPlayer> &GetInstance() {
		// BLZR_CORE_INFO("Sound player reference");
		if (instance == nullptr) {
			instance = std::make_shared<SoundPlayer>();
		}
		return instance;
	}

	SoundPlayer();
	~SoundPlayer();

	static void CreateLuaEntityBind(sol::state_view &lua);
	static void CreateLuaSoundPlayer(sol::state &lua, Registry &registry);

	void PlayMusic(const std::string &name, int loop);
	void PlayMusicFadeIn(const std::string &name, int loop, int fadeInEffect);
	void BLZR_API MusicVolume(std::string name, const int volume);
	int BLZR_API GetCurrentMusicVolume();
	void MuteMusic();
	void ToggleMusicMute();
	void ToggleMusic(const std::string &name);
	void PlayEffect(const std::string &name, int loop);
	void ToggleEffect(int channel);
	void BLZR_API EffectVolume(const int channel, const int volume);
	void MuteEffect(int channel);
	void ToggleMuteEffect(int channel);
	void ToggleMuteAllEffects();
	int GetEffectVolume(int channel);
	bool isChannelPlaying(int channel);
	Ref<Music> BLZR_API GetCurrentPlaying();

	void effectTest(int channel, int value);
};

} // namespace Blazr
