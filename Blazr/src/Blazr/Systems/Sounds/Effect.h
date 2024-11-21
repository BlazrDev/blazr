#pragma once
#include <SDL_mixer.h>
#include "SoundProperties.h"

namespace Blazr {
    class Effect {
        private:
            SoundProperties m_Params{};
            Mix_Chunk* sample;
        
        public:
            Effect(const SoundProperties& params, Mix_Chunk* s);
            ~Effect() = default;

            Mix_Chunk* getSample();
    };
}