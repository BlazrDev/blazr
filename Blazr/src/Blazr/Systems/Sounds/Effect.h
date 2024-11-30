#pragma once
#include "SoundProperties.h"
#include <SDL_mixer.h>

namespace Blazr {
class Effect {
  private:
	SoundProperties m_Params{};
	Mix_Chunk *sample;
	int channel;

  public:
	Effect(const SoundProperties &params, Mix_Chunk *s, int channel);
	~Effect() = default;
	int getChannel() const;
	Mix_Chunk *getSample();
};
} // namespace Blazr
