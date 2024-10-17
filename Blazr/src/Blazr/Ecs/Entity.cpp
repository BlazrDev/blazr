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
