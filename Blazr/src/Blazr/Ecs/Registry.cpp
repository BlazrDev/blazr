#include "blzrpch.h"
#include "Entity.h"
#include "MetaUtil.h"
#include "Registry.h"
#include "sol.hpp"

Blazr::Registry::Registry() { m_Registry = std::make_unique<entt::registry>(); }

void Blazr::Registry::CreateLuaRegistryBind(sol::state_view &lua,
											Registry &registry) {
	using namespace entt::literals;
	lua.new_usertype<entt::runtime_view>(
		"runtime_view", sol::no_constructor, "for_each",
		[&](const entt::runtime_view &view, sol::function func,
			sol::this_state s) {
			if (!func.valid()) {
				return;
			}
			for (auto entity : view) {
				Entity e{registry, entity};
				func(e);
			}
		},
		"exclude",
		[&](entt::runtime_view &view, const sol::variadic_args &args) {
			for (const auto &type : args) {
				if (type.as<sol::table>().valid()) {
					const auto excluded = InvokeMeta(
						GetIdType(type), "exclude_component_from_view"_hs,
						&registry, view);
					view =
						excluded ? excluded.cast<entt::runtime_view>() : view;
				}
			};
		});
	lua.new_usertype<Registry>(
		"Registry", sol::no_constructor, "get_entities",
		[&](const sol::variadic_args &args) {
			entt::runtime_view view{};

			for (const auto &type : args) {
				if (type.as<sol::table>().valid()) {
					const auto included =
						InvokeMeta(GetIdType(type), "add_component_to_view"_hs,
								   &registry, view);
					view =
						included ? included.cast<entt::runtime_view>() : view;
				}
			}
			return view;
		},
		"clear", [&]() { registry.GetRegistry().clear(); });
}
