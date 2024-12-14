#pragma once
#include "Blazr/Ecs/Entity.h"
#include <json.hpp>
#include <string>

namespace Blazr {
class Layer {
  public:
	std::string name;
	int zIndex; // rendering order
	std::vector<Ref<Entity>> entities;
	static bool showColliders;

	Layer(const std::string &name, int zIndex) : name(name), zIndex(zIndex) {}

	void AddEntity(Ref<Entity> entity);
	void RemoveEntity(Ref<Entity> entity);
	void RemoveEntity(const std::string &name);

	void Render(Registry &registry);

	static void BindLayer(sol::state &lua);

	static void to_json(nlohmann::json &j, const Ref<Layer> layer);
	static void from_json(const nlohmann::json &j, Ref<Layer> layer);
};
} // namespace Blazr
