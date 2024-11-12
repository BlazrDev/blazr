#pragma once

#include "Music.h"
#include "Blazr/Core/Core.h"
#include "Blazr/Core/Log.h"
#include "entt.hpp"
#include "sol.hpp"
namespace Blazr {

    class SoundPlayer {

        private:
            static Ref<SoundPlayer> instance;


        public:
            static Ref<SoundPlayer> &GetInstance() {
                BLZR_CORE_INFO("Sound player reference");
                if(instance == nullptr) {
                    instance = std::make_shared<SoundPlayer>();
                }
                return instance;
            }

            SoundPlayer();
            ~SoundPlayer();


            static void CreateLuaEntityBind(sol::state_view &lua);

            
            void PlayMusic(const std::string &name, int loop);
            void PlayMusicFadeIn(const std::string &name, int loop, int fadeInEffect);
            void ToggleMusic(const std::string &name);
            void PlayEffect(const std::string &name, int loop, int channel);
            void ToggleEffect(int channel);
            bool isChannelPlaying(int channel);
    };

}