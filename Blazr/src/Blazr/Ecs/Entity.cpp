#include "Components/Identification.h"
#include "Entity.h"

Blazr::Entity::Entity(Registry &registry)
	: Blazr::Entity(registry, "GameObject", "") {}

Blazr::Entity::Entity(Registry &registry, const std::string &name,
					  const std::string &group)
	: m_Registry(registry), m_Name(name), m_Group(group) {
	AddComponent<Identification>(
		Identification{.name = name,
					   .group = group,
					   .id = static_cast<int32_t>(m_EntityHandler)});
}
Blazr::Entity::Entity(Registry &registry, const entt::entity &entity)
	: m_Registry(registry), m_EntityHandler(entity), m_Name(""), m_Group("") {
	if (m_Registry.GetRegistry().all_of<Identification>(m_EntityHandler)) {
		auto id = GetComponent<Identification>();
		m_Name = id.name;
		m_Group = id.group;
	}
}
