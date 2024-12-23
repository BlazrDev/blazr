#pragma once

#include "blzrpch.h"
#include "Blazr/Ecs/Entity.h"
#include "Blazr/Renderer/Camera2D.h"

namespace Blazr {
class FollowCamera {
  private:
	glm::vec2 m_Position{0.f};
	Camera2D &m_Camera;
	Ref<Entity> m_Target;

  public:
	FollowCamera(Camera2D &camera, Ref<Entity> target);
	~FollowCamera() = default;

	Camera2D &GetCamera() { return m_Camera; }

	Ref<Entity> GetTarget() { return m_Target; }

	void SetTarget(Ref<Entity> target) { m_Target = target; }

	void Update();

	static void to_json(nlohmann::json &j, const FollowCamera &followCamera);
	static void from_json(const nlohmann::json &j, FollowCamera &followCamera,
						  Camera2D &camera);
};
} // namespace Blazr
