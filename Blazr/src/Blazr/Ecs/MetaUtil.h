#pragma once
#include "Blazr/Core/Log.h"
#include "Blazr/Ecs/Entity.h"
#include "entt.hpp"
#include "sol.hpp"
#include <utility>

namespace Blazr {
[[nodiscard]] entt::id_type GetIdType(const sol::table &comp);

template <typename... Args>
inline auto InvokeMeta(entt::meta_type meta, entt::id_type id, Args &&...args) {
	if (!meta) {
		BLZR_CORE_ERROR("Meta type is null!");
		assert(false && "Meta type is null!");
		return entt::meta_any{};
	}

	if (auto function = meta.func(id); function) {
		BLZR_CORE_INFO("Function found!");
		auto result = function.invoke({}, std::forward<Args>(args)...);

		if (result) {
			BLZR_CORE_INFO("Function invoked successfully, result is valid.");
		} else {
			BLZR_CORE_ERROR("Function invoked, but result is null.");
		}
		return result;
	}

	return entt::meta_any{};
}
template <typename... Args>
inline auto InvokeMeta(entt::id_type id, entt::id_type func_id,
					   Args &&...args) {
	return InvokeMeta(entt::resolve(id), func_id, std::forward<Args>(args)...);
}
} // namespace Blazr
