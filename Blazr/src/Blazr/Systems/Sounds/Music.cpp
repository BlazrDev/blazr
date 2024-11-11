#include "Music.h"


Blazr::Music::Music(const SoundProperties& params, Mix_Music* s) : m_Params{params}, sample{s} {

};

Mix_Music* Blazr::Music::getSample() {
    return this->sample;
};


