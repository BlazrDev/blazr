#include "blzrpch.h"
#include "SoundPlayer.h"
#include "Blazr/Core/Log.h"
#include "Music.h"
#include "Blazr/Resources/AssetManager.h"
#include <string>



Ref<Blazr::SoundPlayer> Blazr::SoundPlayer::instance = nullptr;

int Blazr::SoundPlayer::NUM_OF_CHANNELS = 16;

Blazr::SoundPlayer::SoundPlayer() {

    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
        std::string error(Mix_GetError());
        BLZR_CORE_ERROR("Unable to open SDL Music Mixer {0}", error);
    }

    BLZR_CORE_INFO("Channels allocated {0}", Mix_AllocateChannels(NUM_OF_CHANNELS));

    for(int i = 1; i <= NUM_OF_CHANNELS; i++) {
        channelVolumes.emplace(i, MIX_MAX_VOLUME);
        channelMuted.emplace(i, false);
    }
    musicVolume = MIX_MAX_VOLUME;
};



Blazr::SoundPlayer::~SoundPlayer() {
    Mix_HaltMusic();
    Mix_Quit();
    BLZR_CORE_INFO("Music SoundPlayer closed!");
}



void Blazr::SoundPlayer::CreateLuaEntityBind(sol::state_view &lua) {
    lua.new_usertype<SoundPlayer>(
        "SoundPlayer",
        sol::constructors<SoundPlayer()>(),
        "play_music", &SoundPlayer::PlayMusicFadeIn,
        "toggle_music", &SoundPlayer::ToggleMusic,
        "set_music_volume", &SoundPlayer::MusicVolume,
        "get_music_volume", &SoundPlayer::GetCurrentMusicVolume,
        "toggle_music_mute", &SoundPlayer::ToggleMusicMute,
        "play_effect", &SoundPlayer::PlayEffect,
        "toggle_effect", &SoundPlayer::ToggleEffect,
        "set_channel_volume", &SoundPlayer::EffectVolume,
        "get_channel_volume", &SoundPlayer::GetEffectVolume,
        "mute_effect", &SoundPlayer::MuteEffect,
        "toggle_mute_effect", &SoundPlayer::ToggleMuteEffect,
        "mute_all_effects", &SoundPlayer::ToggleMuteAllEffects,
        "is_playing", &SoundPlayer::isChannelPlaying,
        "effect_test", &SoundPlayer::effectTest,
        "get_instance", []() { return SoundPlayer::GetInstance().get(); }
    );
}



void Blazr::SoundPlayer::PlayMusic(const std::string &name, int loop) {

    auto assetManager = AssetManager::GetInstance();

    auto music = assetManager->GetMusic(name);
    if(!music->getSample()) {
        BLZR_CORE_ERROR("Mix_Music is null");
        return;
    }

    if(Mix_PlayMusic(music->getSample(), loop) == -1) {
        std::string error(Mix_GetError());
        BLZR_CORE_ERROR("Failed to play music: {0}", error);
    }


}

void Blazr::SoundPlayer::PlayMusicFadeIn(const std::string &name, int loop, int fadeInEffect) {
    auto assetManager = AssetManager::GetInstance();

    auto music = assetManager->GetMusic(name);
    if(!music->getSample()) {
        BLZR_CORE_ERROR("Mix_Music is null");
        return;
    }

    if(Mix_FadeInMusic(music->getSample(), loop, fadeInEffect) == -1) {
        std::string error(Mix_GetError());
        BLZR_CORE_ERROR("Failed to play faded music: {0}", error);
    }


}

void Blazr::SoundPlayer::ToggleMusic(const std::string &name) {
    if(Mix_PausedMusic()) {
        Mix_ResumeMusic();
    } else {
        Mix_PausedMusic();
    }
}

void Blazr::SoundPlayer::MusicVolume(const int volume) {
    int scaledVolume = static_cast<int>(volume * (MIX_MAX_VOLUME / 100.0));
    musicVolume = std::max(0, std::min(scaledVolume, MIX_MAX_VOLUME));

    Mix_VolumeMusic(musicVolume);
}

int Blazr::SoundPlayer::GetCurrentMusicVolume() {
    return Mix_VolumeMusic(-1);
}

void Blazr::SoundPlayer::MuteMusic() {
    Mix_VolumeMusic(0);
}


void Blazr::SoundPlayer::ToggleMusicMute() {
    int currentVolume = Mix_VolumeMusic(-1);
    if(currentVolume > 0) {
        Mix_VolumeMusic(0);
    } else {
        Mix_VolumeMusic(musicVolume);
    }
}



void Blazr::SoundPlayer::PlayEffect(const std::string &name, int loop, int channel) {
    auto assetManager = AssetManager::GetInstance();
    auto effect = assetManager->GetEffect(name);
    if(!effect->getSample()) {
        BLZR_CORE_ERROR("Mix_Chunk is null");
        return;
    }
    if(Mix_PlayChannel(channel, effect->getSample(), loop) == -1) {
        std::string error(Mix_GetError());
        BLZR_CORE_ERROR("Failed to play effect: {0}", error);
    }
}

void Blazr::SoundPlayer::ToggleEffect(int channel) {
    if(Mix_Paused(channel)) {
        Mix_Resume(channel);
    } else {
        Mix_Pause(channel);
    }
}


void Blazr::SoundPlayer::EffectVolume(const int channel, const int volume) {
    int scaledVolume = static_cast<int>(volume * (MIX_MAX_VOLUME / 100.0));
    int clampedVolume = std::max(0, std::min(scaledVolume, MIX_MAX_VOLUME));
    int res = Mix_Volume(channel, clampedVolume);
    BLZR_CORE_INFO("test {0}", res);
    channelVolumes[channel] = clampedVolume;
}

int Blazr::SoundPlayer::GetEffectVolume(int channel) { 
    if(channelVolumes.find(channel) != channelVolumes.end()) {
        return channelVolumes[channel];
    }
    else {
        BLZR_CORE_ERROR("Channel {0} not found! ", channel);
        return -1;
    }
}

void Blazr::SoundPlayer::MuteEffect(int channel) {
    Mix_Volume(channel, 0);
}

void Blazr::SoundPlayer::ToggleMuteEffect(int channel) {
    if(channelMuted.find(channel) != channelMuted.end()) {
        if(channelMuted[channel]) {
            Mix_Volume(channel, channelVolumes[channel]);
            channelMuted[channel] = false;
        } else {
            Mix_Volume(channel, 0);
            channelMuted[channel] = true;
        }
    }
}

void Blazr::SoundPlayer::ToggleMuteAllEffects() {
    for(int i = 1; i <= NUM_OF_CHANNELS; i++) {
        ToggleMuteEffect(i);
    }
}

bool Blazr::SoundPlayer::isChannelPlaying(int channel) {
    return Mix_Playing(channel);

}


void Blazr::SoundPlayer::effectTest(int channel, int value) {
    Mix_SetDistance(channel, value);
}

