#include "blzrpch.h"
#include "Effect.h"

int Blazr::Effect::s_channel = 1;
Blazr::Effect::Effect(const SoundProperties &params, Mix_Chunk *s)
	: m_Params{params}, sample{s}, channel{s_channel++} {}

Mix_Chunk *Blazr::Effect::getSample() { return this->sample; }

int Blazr::Effect::getChannel() const { return channel; }
