#pragma once
#include "Blazr/Ecs/Entity.h"
#include <nlohmann/json.hpp>
#include <string>

namespace Blazr {
class Layer {
  public:
	std::string name;
	int zIndex; // rendering order
	std::vector<Ref<Entity>> entities;

	Layer(const std::string &name, int zIndex) : name(name), zIndex(zIndex) {}

	void AddEntity(Ref<Entity> entity);
	void RemoveEntity(Ref<Entity> entity);

	void Render(Registry &registry);

	static void BindLayer(sol::state &lua);

	// TODO add serialization and deserialization
};
} // namespace Blazr
