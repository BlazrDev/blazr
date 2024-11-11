#include "Registry.h"
#include "entt.hpp"
#include "sol.hpp"

namespace Blazr {
class Entity {
  public:
	Entity(Registry &registry);
	BLZR_API Entity(Registry &registry, const std::string &name = "",
		   const std::string &group = "");

	Entity(Registry &registry, const entt::entity &entity);
	~Entity() = default;

	inline const std::string &GetName() const { return m_Name; }
	inline const std::string &GetGroup() const { return m_Group; }

	inline entt::registry &GetRegistry() { return m_Registry.GetRegistry(); }

	static void CreateLuaEntityBind(sol::state_view &lua, Registry &registry);

	template <typename TComponent> static void RegisterMetaComponent();

	inline entt::entity GetEntityHandler() const { return m_EntityHandler; }
	inline std::uint32_t destroy() {
		return m_Registry.GetRegistry().destroy(m_EntityHandler);
	}

	template <typename TComponent, typename... Args>
	TComponent &AddComponent(Args &&...args) {
		return m_Registry.GetRegistry().emplace<TComponent>(
			m_EntityHandler, std::forward<Args>(args)...);
	}

	template <typename TComponent, typename... Args>
	TComponent &ReplaceComponent(Args &&...args) {
		auto &registry = m_Registry.GetRegistry();
		if (registry.all_of<TComponent>(m_EntityHandler)) {
			return registry.replace<TComponent>(m_EntityHandler,
												std::forward<Args>(args)...);
		} else {
			return registry.emplace<TComponent>(m_EntityHandler,
												std::forward<Args>(args)...);
		}
	}
	template <typename TComponent> TComponent &GetComponent() {
		return m_Registry.GetRegistry().get<TComponent>(m_EntityHandler);
	}

	template <typename TComponent> bool HasComponent() {
		return m_Registry.GetRegistry().all_of<TComponent>(m_EntityHandler);
	}

	template <typename TComponent> void RemoveComponent() {
		m_Registry.GetRegistry().remove<TComponent>(m_EntityHandler);
	}

	template <typename TComponent>
	auto add_component(Entity &entity, const sol::table &comp,
					   sol::this_state s) {
		auto component = entity.AddComponent<TComponent>(
			comp.valid() ? comp.as<TComponent>() : TComponent{});
		return sol::make_reference(s, std::ref(component));
	}

  private:
	Registry &m_Registry;
	entt::entity m_EntityHandler;
	std::string m_Name;
	std::string m_Group;
};
} // namespace Blazr
