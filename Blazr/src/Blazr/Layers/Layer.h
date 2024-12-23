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
	static bool BLZR_API showColliders;

	Layer(const std::string &name, int zIndex) : name(name), zIndex(zIndex) {}

	void BLZR_API AddEntity(Ref<Entity> entity);
	void BLZR_API RemoveEntity(Ref<Entity> entity);
	void BLZR_API RemoveEntity(const std::string &name);

	std::vector<Ref<Entity>> BLZR_API GetEntities() { return entities; }

	void Render(Registry &registry);

	static void BLZR_API BindLayer(sol::state &lua);

	static void to_json(nlohmann::json &j, const Ref<Layer> layer);
	static void from_json(const nlohmann::json &j, Ref<Layer> layer,
						  Ref<sol::state> luaState);
};
} // namespace Blazr
