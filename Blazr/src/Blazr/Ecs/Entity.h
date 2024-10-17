#include "Registry.h"
#include "entt.hpp"

namespace Blazr {
class Entity {
  public:
	Entity(Registry &registry);
	Entity(Registry &registry, const std::string &name = "",
		   const std::string &group = "");
	~Entity() = default;

	inline entt::registry &GetRegistry() { return m_Registry.GetRegistry(); }
	inline entt::entity GetEntityHandler() const { return m_EntityHandler; }
	inline const std::string &GetName() const { return m_Name; }
	inline const std::string &GetGroup() const { return m_Group; }
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

  private:
	Registry &m_Registry;
	entt::entity m_EntityHandler;
	std::string m_Name;
	std::string m_Group;
};
} // namespace Blazr
