#pragma once

#include "Blazr/Ecs/Entity.h"
#include "Blazr/Ecs/Registry.h"
#include "Blazr/Renderer/Camera2D.h"
#include "Blazr/Renderer/Texture2D.h"
#include <json.hpp>
#include <memory>

using namespace nlohmann;
namespace Blazr {

class Scene {
  public:
	BLZR_API Scene();
	BLZR_API ~Scene();

	entt::entity AddEntity();
	void RemoveEntity(entt::entity entity);

	std::shared_ptr<Registry> GetRegistry() { return m_Registry; }
	Camera2D &GetCamera() { return m_Camera; }

	void BLZR_API Update();
	void BLZR_API Render();
	void Serialize(json &j) const;
	void Deserialize(const json &j);

  private:
	std::shared_ptr<Blazr::Registry> m_Registry;
	Camera2D m_Camera;
};

} // namespace Blazr
