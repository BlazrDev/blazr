#include "blzrpch.h"
#include "MetaUtil.h"

entt::id_type Blazr::GetIdType(const sol::table &comp) {
	if (!comp.valid()) {
		BLZR_CORE_ERROR("Component has not been exposed to lua");
		assert(comp.valid() && "Component has not been exposed to lua");
		return -1;
	}

	const auto func = comp["type_id"].get<sol::function>();

	assert(func.valid() && "Component has no type_id function");
	return func.valid() ? func().get<entt::id_type>() : -1;
}
