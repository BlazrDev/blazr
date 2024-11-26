#include "Blazr/Core/Log.h"
#include "Blazr/Ecs/Registry.h"
#include "Blazr/Physics/Box2DWrapper.h"
#include "PhysicsComponent.h"
#include "box2d/b2_circle_shape.h"
#include "box2d/box2d.h"
#include "sol.hpp"

Blazr::PhysicsComponent::PhysicsComponent(std::shared_ptr<b2World> world,
										  const PhysicsAtributes &atributes)
	: m_World(world), m_Atributes(atributes), m_RigidBody(nullptr) {}

Blazr::PhysicsComponent::PhysicsComponent() : m_Atributes(PhysicsAtributes{}) {}
void Blazr::PhysicsComponent::init(int windowWidth, int windowHeight) {
	if (!m_World) {
		BLZR_CORE_ERROR("PhysicsComponent::init: PhysicsWorld is nullptr");
		return;
	}

	bool bCircle{m_Atributes.isCircle};
	b2BodyDef bodyDef{};
	bodyDef.type = static_cast<b2BodyType>(m_Atributes.type);
	// bodyDef.type = b2BodyType::b2_dynamicBody;
	//
	BLZR_CORE_ERROR("PhysicsComponent::init window size: {0} {1}", windowWidth,
					windowHeight);

	bodyDef.position.Set((m_Atributes.position.x - (windowWidth * 0.5f) +
						  m_Atributes.boxSize.x * m_Atributes.scale.x * 0.5f) *
							 PIXELS_TO_METERS,
						 (m_Atributes.position.y - (windowHeight * 0.5f) +
						  m_Atributes.boxSize.y * m_Atributes.scale.y * 0.5f) *
							 PIXELS_TO_METERS);

	BLZR_CORE_ERROR("PhysicsComponent::init: {0}, {1}", bodyDef.position.x,
					bodyDef.position.y);

	bodyDef.gravityScale = m_Atributes.gravityScale;
	bodyDef.fixedRotation = m_Atributes.isFixedRotation;

	m_RigidBody = Blazr::CreateSharedBody(m_World->CreateBody(&bodyDef));

	if (!m_RigidBody) {
		BLZR_CORE_ERROR("PhysicsComponent::init: Failed to create rigid body");
		return;
	}

	b2CircleShape circleShape;
	b2PolygonShape boxShape;

	if (m_Atributes.isCircle) {
		circleShape.m_radius =
			PIXELS_TO_METERS * m_Atributes.radius * m_Atributes.scale.x;
	} else if (m_Atributes.isBoxShape) {
		boxShape.SetAsBox(PIXELS_TO_METERS * m_Atributes.boxSize.x *
							  m_Atributes.scale.x * 0.5f,
						  PIXELS_TO_METERS * m_Atributes.boxSize.y *
							  m_Atributes.scale.y * 0.5f,
						  b2Vec2{m_Atributes.offset.x * PIXELS_TO_METERS,
								 m_Atributes.offset.y * PIXELS_TO_METERS},
						  0.0f);

		BLZR_CORE_ERROR(
			"BoxShape::init: {0}, {1}",
			PIXELS_TO_METERS * (m_Atributes.boxSize.x + m_Atributes.offset.x) *
				m_Atributes.scale.x * 0.5f,
			PIXELS_TO_METERS * (m_Atributes.boxSize.y + m_Atributes.offset.y) *
				m_Atributes.scale.y * 0.5f);
		// // boxShape.SetAsBox(50, 50);
		// BLZR_CORE_ERROR("BoxShape::init: {0}, {1}",
		// 				PIXELS_TO_METERS * m_Atributes.boxSize.x *
		// 					m_Atributes.scale.x * 0.5f,
		// 				PIXELS_TO_METERS * m_Atributes.boxSize.y *
		// 					m_Atributes.scale.y * 0.5f);
	}

	// b2Polygon box = b2MakeBox(1.0f, 1.0f);
	b2FixtureDef fixtureDef{};
	if (bCircle)
		fixtureDef.shape = &circleShape;
	else
		fixtureDef.shape = &boxShape;

	fixtureDef.density = m_Atributes.density;
	fixtureDef.friction = m_Atributes.friction;
	fixtureDef.restitution = m_Atributes.restitution;
	fixtureDef.restitutionThreshold = m_Atributes.restitutionThreshold;
	fixtureDef.isSensor = m_Atributes.isSensor;
	// fixtureDef.userData.pointer =
	// 	reinterpret_cast<uintptr_t>(m_pUserData.get());

	auto pFixture = m_RigidBody->CreateFixture(&fixtureDef);
	if (!pFixture) {
		BLZR_CORE_ERROR("Failed to create the rigid body fixture!");
	}
}
void Blazr::PhysicsComponent::CreateLuaPhysicsComponentBind(
	sol::state_view &lua, Blazr::Registry &registry) {
	lua.new_enum<Blazr::RigidBodyType>(
		"RigidBodyType", {{"Static", Blazr::RigidBodyType::STATIC},
						  {"Kinematic", Blazr::RigidBodyType::KINEMATIC},
						  {"Dynamic", Blazr::RigidBodyType::DYNAMIC}});

	lua.new_usertype<Blazr::PhysicsAtributes>(
		"PhysicsAtributes", sol::call_constructor,
		sol::factories(
			[] { return Blazr::PhysicsAtributes{}; },
			[](const sol::table phyAttr) {
				return Blazr::PhysicsAtributes{
					.type =
						phyAttr["type"].get_or(Blazr::RigidBodyType::STATIC),
					.density = phyAttr["density"].get_or(100.f),
					.friction = phyAttr["friction"].get_or(0.2f),
					.restitution = phyAttr["restitution"].get_or(0.2f),
					.restitutionThreshold =
						phyAttr["restitutionThreshold"].get_or(0.2f),
					.radius = phyAttr["radius"].get_or(0.f),
					.gravityScale = phyAttr["gravityScale"].get_or(1.f),
					.position = glm::vec2{phyAttr["position"]["x"].get_or(0.f),
										  phyAttr["position"]["y"].get_or(0.f)},
					.scale = glm::vec2{phyAttr["scale"]["x"].get_or(0.f),
									   phyAttr["scale"]["y"].get_or(0.f)},
					.boxSize = glm::vec2{phyAttr["boxSize"]["x"].get_or(0.f),
										 phyAttr["boxSize"]["y"].get_or(0.f)},
					.offset = glm::vec2{phyAttr["offset"]["x"].get_or(0.f),
										phyAttr["offset"]["y"].get_or(0.f)},
					.isSensor = phyAttr["isSensor"].get_or(false),
					.isFixedRotation = phyAttr["isFixedRotation"].get_or(true),
					.isCircle = phyAttr["isCircle"].get_or(false),
					.isBoxShape = phyAttr["isBoxShape"].get_or(true),
					.filterCategory =
						phyAttr["filterCategory"].get_or((uint16_t)0),
					.filterMask = phyAttr["filterMask"].get_or((uint16_t)0),
					.filterGroup = phyAttr["filterGroup"].get_or((int16_t)0)};
			}),
		"type", &Blazr::PhysicsAtributes::type, "density",
		&Blazr::PhysicsAtributes::density, "friction",
		&Blazr::PhysicsAtributes::friction, "restitution",
		&Blazr::PhysicsAtributes::restitution, "restitutionThreshold",
		&Blazr::PhysicsAtributes::restitutionThreshold, "radius",
		&Blazr::PhysicsAtributes::radius, "gravityScale",
		&Blazr::PhysicsAtributes::gravityScale, "position",
		&Blazr::PhysicsAtributes::position, "scale",
		&Blazr::PhysicsAtributes::scale, "boxSize",
		&Blazr::PhysicsAtributes::boxSize, "offset",
		&Blazr::PhysicsAtributes::offset, "isSensor",
		&Blazr::PhysicsAtributes::isSensor, "isFixedRotation",
		&Blazr::PhysicsAtributes::isFixedRotation, "isCircle",
		&Blazr::PhysicsAtributes::isCircle, "isBoxShape",
		&Blazr::PhysicsAtributes::isBoxShape, "filterCategory",
		&Blazr::PhysicsAtributes::filterCategory, "filterMask",
		&Blazr::PhysicsAtributes::filterMask, "filterGroup",
		&Blazr::PhysicsAtributes::filterGroup, "set_transform",
		[](PhysicsComponent &pc, const glm::vec2 &position) {
			auto body = pc.GetRigidBody();
			if (!body) {
				// TODO: Add Error
				return;
			}

			// auto &engineData = CoreEngineData::GetInstance();
			// const auto p2m = engineData.PixelsToMeters();

			const auto scaleHalfHeight = 1280 * 0.5f / METERS_TO_PIXELS;
			const auto scaleHalfWidth = 720 * 0.5f / METERS_TO_PIXELS;

			auto bx = (position.x * PIXELS_TO_METERS) - scaleHalfWidth;
			auto by = (position.y * PIXELS_TO_METERS) - scaleHalfHeight;

			// auto bx = position.x;
			// auto by = position.y;

			BLZR_CORE_INFO("PhysicsComponent::set_transform: {0}, {1}", bx, by);

			body->SetTransform(b2Vec2{bx, by}, 0.f);
		});

	auto &physicsWorld = registry.GetContext<std::shared_ptr<b2World>>();

	if (!physicsWorld) {
		return;
	}
	lua.new_usertype<Blazr::PhysicsComponent>(
		"PhysicsComponent", "type_id",
		&entt::type_hash<Blazr::PhysicsComponent>::value, sol::call_constructor,
		sol::factories([&](const Blazr::PhysicsAtributes &attr) {
			Blazr::PhysicsComponent pc{physicsWorld, attr};
			pc.init(1280, 720); // TODO: Change based on window values
			return pc;
		}),

		"set_linear_velocity",
		[](PhysicsComponent &pc, const glm::vec2 &velocity) {
			auto body = pc.GetRigidBody();
			if (!body) {
				// TODO: Add Error
				return;
			}

			body->SetLinearVelocity(b2Vec2{velocity.x, velocity.y});
		},
		"get_linear_velocity",
		[](PhysicsComponent &pc) {
			auto body = pc.GetRigidBody();
			if (!body) {
				// TODO: Add Error
				return glm::vec2{0.f};
			}
			const auto &linearVelocity = body->GetLinearVelocity();
			return glm::vec2{linearVelocity.x, linearVelocity.y};
		},
		"set_angular_velocity",
		[](PhysicsComponent &pc, float angularVelocity) {
			auto body = pc.GetRigidBody();
			if (!body) {
				// TODO: Add Error
				return;
			}

			body->SetAngularVelocity(angularVelocity);
		},
		"set_linear_impulse",
		[](PhysicsComponent &pc, const glm::vec2 &impulse) {
			auto body = pc.GetRigidBody();
			if (!body) {
				// TODO: Add Error
				return;
			}

			body->ApplyLinearImpulse(b2Vec2{impulse.x, impulse.y},
									 body->GetPosition(), true);
		}

	);
}
