#include "blzrpch.h"
#include "Effect.h"

Blazr::Effect::Effect(const SoundProperties &params, Mix_Chunk *s, int channel)
	: m_Params{params}, sample{s}, channel{channel} {}

Mix_Chunk *Blazr::Effect::getSample() { return this->sample; }

int Blazr::Effect::getChannel() const { return channel; }
