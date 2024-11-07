#pragma once
#include "Blazr/Core/Log.h"
#include "Blazr/Ecs/Entity.h"
#include "entt.hpp"
#include "sol.hpp"
#include <utility>

namespace Blazr {
[[nodiscard]] entt::id_type GetIdType(const sol::table &comp);

// template <typename... Args>
inline auto InvokeMeta(entt::meta_type meta, entt::id_type id, Entity &entity,
					   const sol::table &comp, sol::this_state s) {
	if (!meta) {
		BLZR_CORE_ERROR("Meta type is null!");
		assert(false && "Meta type is null!");
		return entt::meta_any{};
	}

	BLZR_CORE_INFO("Invoking meta function with id: {0}", id);
	BLZR_CORE_INFO("Meta type: {0}", meta.info().name());
	if (auto function = meta.func(id); function) {
		BLZR_CORE_INFO("Function found!");
		auto result = function.invoke({}, entity, comp, s);

		if (result) {
			BLZR_CORE_INFO("Function invoked successfully, result is valid.");
		} else {
			BLZR_CORE_ERROR("Function invoked, but result is null.");
		}
		return result;
	}

	return entt::meta_any{};
}
// template <typename... Args>
inline auto InvokeMeta(entt::id_type id, entt::id_type func_id, Entity &entity,
					   const sol::table &comp, sol::this_state s) {
	return InvokeMeta(entt::resolve(id), func_id, entity, comp, s);
}
} // namespace Blazr
