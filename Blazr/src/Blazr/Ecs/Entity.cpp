#include "blzrpch.h"
#include "Blazr/Core/Log.h"
#include "Blazr/Ecs/Registry.h"
#include "Components/Identification.h"
#include "Entity.h"
#include "MetaUtil.h"
#include "sol.hpp"
#include <string>

int Blazr::Entity::ID = 0;

Blazr::Entity::Entity(Registry &registry)
	: Blazr::Entity(registry, "GameObject " + std::to_string(++ID), "") {}

Blazr::Entity::Entity(Registry &registry, const std::string &name,
					  const std::string &group)
	: m_Registry(registry), m_Name(name), m_Group(group),
	  m_EntityHandler{registry.CreateEntity()} {
	AddComponent<Identification>(
		Identification{.name = name,
					   .group = group,
					   .id = static_cast<int32_t>(m_EntityHandler)});

	// BLZR_CORE_INFO("Entity created with name: {0}, group: {1}", name, group);
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
			return CreateRef<Entity>(registry, name, group);
		}),
		"name", &Entity::GetName, "group", &Entity::GetGroup, "id",
		[](Entity &entity) {
			return static_cast<int32_t>(entity.GetEntityHandler());
		},
		"destroy", &Entity::destroy, "set_name", &Entity::SetName,
		"add_component",
		[](Entity &entity, const sol::table &comp,
		   sol::this_state s) -> sol::object {
			if (!comp.valid()) {
				return sol::lua_nil_t();
			}

			const auto component = InvokeMeta(
				GetIdType(comp), "add_component"_hs, entity, comp, s);

			return component ? component.cast<sol::reference>()
							 : sol::lua_nil_t{};
		},
		"has_component",
		[](Entity &entity, const sol::table &comp) {
			const auto component =
				InvokeMeta(GetIdType(comp), "has_component"_hs, entity);

			return component ? component.cast<bool>() : false;
		},
		"get_component",
		[](Entity &entity, const sol::table &comp, sol::this_state s) {
			const auto component =
				InvokeMeta(GetIdType(comp), "get_component"_hs, entity, s);

			return component ? component.cast<sol::reference>()
							 : sol::lua_nil_t{};
		},

		"remove_component",
		[](Entity &entity, const sol::table &comp) {
			const auto component =
				InvokeMeta(GetIdType(comp), "remove_component"_hs, entity);

			return component ? component.cast<sol::reference>()
							 : sol::lua_nil_t{};
		});

	// lua.new_usertype<Ref<Entity>>(
	// 	"EntityRef", "get", [](Ref<Entity> &ref) -> Entity & { return *ref; });
}
