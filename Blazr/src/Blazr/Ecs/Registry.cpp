#include "Registry.h"

Blazr::Registry::Registry() { m_Registry = std::make_unique<entt::registry>(); }
