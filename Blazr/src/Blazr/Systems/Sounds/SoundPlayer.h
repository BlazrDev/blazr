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

  public:
	std::unordered_map<int, int> channelVolumes;
	int musicVolume;
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
	void MusicVolume(const int volume);
	int GetCurrentMusicVolume();
	void MuteMusic();
	void ToggleMusicMute();
	void ToggleMusic(const std::string &name);
	void PlayEffect(const std::string &name, int loop);
	void ToggleEffect(int channel);
	void EffectVolume(const int channel, const int volume);
	void MuteEffect(int channel);
	void ToggleMuteEffect(int channel);
	void ToggleMuteAllEffects();
	int GetEffectVolume(int channel);
	bool isChannelPlaying(int channel);

	void effectTest(int channel, int value);
};

} // namespace Blazr
