#include "blzrpch.h"
#include "SoundPlayer.h"
#include "Blazr/Core/Log.h"
#include "Music.h"
#include "Blazr/Resources/AssetManager.h"
#include <string>



Ref<Blazr::SoundPlayer> Blazr::SoundPlayer::instance = nullptr;

Blazr::SoundPlayer::SoundPlayer() {

    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
        std::string error(Mix_GetError());
        BLZR_CORE_ERROR("Unable to open SDL Music Mixer {0}", error);
    }

    BLZR_CORE_INFO("Channels allocated {0}", Mix_AllocateChannels(16));

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
        "play_music", &SoundPlayer::PlayMusic,
        "play_music_fade_in", &SoundPlayer::PlayMusicFadeIn,
        "toggle_music", &SoundPlayer::ToggleMusic,
        "play_effect", &SoundPlayer::PlayEffect,
        "toggle_effect", &SoundPlayer::ToggleEffect,
        "is_playing", &SoundPlayer::isChannelPlaying,
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


bool Blazr::SoundPlayer::isChannelPlaying(int channel) {
    return Mix_Playing(channel);
}

