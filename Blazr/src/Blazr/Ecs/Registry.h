#pragma once
#include "Blazr/Core/Core.h"
#include "entt.hpp"
#include "sol.hpp"

namespace Blazr {
class Registry {
  public:
	BLZR_API Registry();
	~Registry() = default;

	inline entt::registry &GetRegistry() { return *m_Registry; }
	inline entt::entity CreateEntity() { return m_Registry->create(); }

	template <typename TContext> TContext &GetContext() {
		return m_Registry->ctx().get<TContext>();
	}

	template <typename TContext> TContext &SetContext() {
		return m_Registry->ctx().emplace<TContext>();
	}

	template <typename TContext> inline bool AddToContext(TContext &context) {
		if (m_Registry->ctx().contains<TContext>()) {
			return false;
		}
		m_Registry->ctx().emplace<TContext>(context);
		return true;
	}

	template <typename TContext> inline bool RemoveContext() {
		return m_Registry->ctx().erase<TContext>();
	}

	static void CreateLuaRegistryBind(sol::state_view &lua, Registry &registry);

	template <typename TComponent>
	static entt::runtime_view &add_component_to_view(Registry *registry,
													 entt::runtime_view &view) {
		return view.iterate(registry->GetRegistry().storage<TComponent>());
	}

	template <typename TComponent>
	static entt::runtime_view &
	exclude_component_from_view(Registry *registry, entt::runtime_view &view) {
		return view.exclude(registry->GetRegistry().storage<TComponent>());
	}

	template <typename TComponent> static void RegisterMetaComponent() {
		using namespace entt::literals;
		entt::meta<TComponent>()
			.type(entt::type_hash<TComponent>::value())
			.template func<&Blazr::Registry::add_component_to_view<TComponent>>(
				"add_component_to_view"_hs)
			.template func<
				&Blazr::Registry::exclude_component_from_view<TComponent>>(
				"exclude_component_from_view"_hs);
	}

  private:
	std::unique_ptr<entt::registry> m_Registry;
};
} // namespace Blazr
