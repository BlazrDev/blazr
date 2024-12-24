#pragma once
#include "Blazr/Core/Core.h"
#include "SoundProperties.h"
#include <SDL_mixer.h>

namespace Blazr {
class Effect {
  private:
	SoundProperties m_Params{};
	Mix_Chunk *sample;
	int channel;
	std::string m_Path;
	static int s_channel;

  public:
	Effect(const SoundProperties &params, Mix_Chunk *s);
	~Effect() = default;
	int BLZR_API getChannel() const;
	SoundProperties GetProperties() const { return m_Params; }
	Mix_Chunk *getSample();
	std::string GetPath() const { return m_Path; }
	void SetPath(const std::string &path) { m_Path = path; }
};
} // namespace Blazr
