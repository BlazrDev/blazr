
#pragma once
#include "../Ecs/Registry.h"
#include "sol.hpp"

namespace Blazr {
class PhysicsSystem {
  private:
	Registry &m_Registry;

  public:
	PhysicsSystem(Registry &registry);
	~PhysicsSystem() = default;

	void Update(Registry &registry);
};
} // namespace Blazr
