#pragma once
#include "Blazr/Ecs/Registry.h"
#include "Blazr/Physics/Box2DWrapper.h"
#include "box2d/box2d.h"
#include "glm.hpp"
#include <cstdint>
#include <memory>
#include <sol.hpp>

namespace Blazr {
enum class RigidBodyType { STATIC = 0, KINEMATIC, DYNAMIC };

constexpr float METERS_TO_PIXELS = 12.f;
constexpr float PIXELS_TO_METERS = 1.f / METERS_TO_PIXELS;

struct PhysicsAtributes {
	RigidBodyType type{RigidBodyType::STATIC};
	float density{1.f}, friction{0.2f}, restitution{0.2f},
		restitutionThreshold{1.f}, radius{0.f}, gravityScale{1.f};

	glm::vec2 position{0.f}, scale{1.f}, boxSize{0.5f, 1.5f}, offset{0.f};
	bool isSensor{false}, isFixedRotation{true}, isCircle{false},
		isBoxShape{true};

	uint16_t filterCategory{0}, filterMask{0};
	int16_t filterGroup{0};
};

class PhysicsComponent {
  private:
	std::shared_ptr<b2World> m_World;
	std::shared_ptr<b2Body> m_RigidBody;
	PhysicsAtributes m_Atributes;

  public:
	PhysicsComponent();
	PhysicsComponent(std::shared_ptr<b2World> world,
					 const PhysicsAtributes &atributes);
	~PhysicsComponent() = default;

	void init(int windowWidth, int windowHeight);
	b2Body *GetRigidBody() const { return m_RigidBody.get(); }

	static void CreateLuaPhysicsComponentBind(sol::state_view &lua,
											  Blazr::Registry &registry);
};
} // namespace Blazr