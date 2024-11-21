#pragma once
#include "SoundProperties.h"
#include <SDL_mixer.h>
namespace Blazr {

    class Music {
        private:
            SoundProperties m_Params{};
            Mix_Music* sample;


        public:
            Music(const SoundProperties& params, Mix_Music* s);
            ~Music() = default;

            Mix_Music* getSample();
    };


}