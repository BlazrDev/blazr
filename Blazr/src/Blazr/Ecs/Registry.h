#pragma once
#include "entt.hpp"

namespace Blazr {
class Registry {
  public:
	Registry();
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

  private:
	std::unique_ptr<entt::registry> m_Registry;
};
} // namespace Blazr
