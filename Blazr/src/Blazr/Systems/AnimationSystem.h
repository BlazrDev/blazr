#pragma once
#include "Blazr/Ecs/Registry.h"

namespace Blazr {
class AnimationSystem {
  public:
	BLZR_API AnimationSystem(Registry &registry);
	~AnimationSystem() = default;

	void Update();

  private:
	Registry &m_Registry;
};
}; // namespace Blazr
