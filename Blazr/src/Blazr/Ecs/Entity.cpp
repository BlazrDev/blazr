#include "blzrpch.h"
#include "Components/Identification.h"
#include "Entity.h"
#include "MetaUtil.h"
#include "sol.hpp"
#include "Blazr/Core/Log.h"

Blazr::Entity::Entity(Registry &registry)
	: Blazr::Entity(registry, "GameObject", "") {}

Blazr::Entity::Entity(Registry &registry, const std::string &name,
					  const std::string &group)
	: m_Registry(registry), m_Name(name), m_Group(group) {
	//AddComponent<Identification>(
	//	Identification{.name = name,
	//				   .group = group,
	//				   .id = static_cast<int32_t>(m_EntityHandler)});
}
Blazr::Entity::Entity(Registry &registry, const entt::entity &entity)
	: m_Registry(registry), m_EntityHandler(entity), m_Name(""), m_Group("") {
	if (m_Registry.GetRegistry().all_of<Identification>(m_EntityHandler)) {
		auto id = GetComponent<Identification>();
		m_Name = id.name;
		m_Group = id.group;
	}
}

void Blazr::Entity::CreateLuaEntityBind(sol::state_view &lua,
										Registry &registry) {
	using namespace entt::literals;
	lua.new_usertype<Entity>(
		"Entity", sol::call_constructor,
		sol::factories([&](const std::string &name, const std::string &group) {
			return Entity(registry, name, group);
		}),
		"add_component",
		[&](Entity &entity, const sol::table &comp,
			sol::this_state s) -> sol::object {
			if (!comp.valid()) {
				return sol::lua_nil_t();
			}

			const auto component = InvokeMeta(
				GetIdType(comp), "add_component"_hs, entity, comp, s);

			return component ? component.cast<sol::reference>()
							 : sol::lua_nil_t{};
		});
}
template <typename TComponent> static void RegisterMetaComponent() {
	using namespace entt::literals;
	entt::meta<TComponent>()
		.type(entt::type_hash<TComponent>::value())
		.template func<&Blazr::Entity::add_component<TComponent>>(
			"add_component"_hs);
};
