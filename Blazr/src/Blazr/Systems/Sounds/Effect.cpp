#include "blzrpch.h"
#include "Effect.h"



Blazr::Effect::Effect(const SoundProperties & params, Mix_Chunk* s) : m_Params{params}, sample{s} {

}

Mix_Chunk* Blazr::Effect::getSample() {
    return this->sample;
}