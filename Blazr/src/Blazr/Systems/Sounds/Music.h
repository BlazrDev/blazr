#pragma once
#include "SoundProperties.h"
#include <SDL_mixer.h>
namespace Blazr {

class Music {
  private:
	SoundProperties m_Params{};
	Mix_Music *sample;
	std::string m_Path;

  public:
	Music(const SoundProperties &params, Mix_Music *s);
	~Music() = default;
	std::string GetPath() { return m_Path; }
	SoundProperties GetProperties() const { return m_Params; }
	void SetPath(const std::string &path) { m_Path = path; }

	Mix_Music *getSample();
};

} // namespace Blazr
