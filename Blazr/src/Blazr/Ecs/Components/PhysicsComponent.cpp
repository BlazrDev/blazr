#include "blzrpch.h"
#include "Blazr/Core/Log.h"
#include "Blazr/Ecs/Registry.h"
#include "Blazr/Physics/Box2DWrapper.h"
#include "PhysicsComponent.h"
#include "box2d/b2_circle_shape.h"
#include "box2d/box2d.h"
#include "sol.hpp"

Blazr::PhysicsComponent::PhysicsComponent(std::shared_ptr<b2World> world,
										  const PhysicsAttributes &atributes)
	: m_World(world), m_Attributes(atributes), m_RigidBody(nullptr) {}

Blazr::PhysicsComponent::PhysicsComponent()
	: m_Attributes(PhysicsAttributes{}) {}
void Blazr::PhysicsComponent::init(int windowWidth, int windowHeight) {
	if (!m_World) {
		BLZR_CORE_ERROR("PhysicsComponent::init: PhysicsWorld is nullptr");
		return;
	}

	b2BodyDef bodyDef{};
	bodyDef.type = static_cast<b2BodyType>(m_Attributes.type);
	// bodyDef.type = b2BodyType::b2_dynamicBody;
	//

	bodyDef.position.Set(
		(m_Attributes.position.x - (windowWidth * 0.5f) +
		 m_Attributes.boxSize.x * m_Attributes.scale.x * 0.5f) *
			PIXELS_TO_METERS,
		(m_Attributes.position.y - (windowHeight * 0.5f) +
		 m_Attributes.boxSize.y * m_Attributes.scale.y * 0.5f) *
			PIXELS_TO_METERS);

	bodyDef.gravityScale = m_Attributes.gravityScale;
	bodyDef.fixedRotation = m_Attributes.isFixedRotation;

	m_RigidBody = Blazr::CreateSharedBody(m_World->CreateBody(&bodyDef));

	if (!m_RigidBody) {
		BLZR_CORE_ERROR("PhysicsComponent::init: Failed to create rigid body");
		return;
	}

	b2CircleShape circleShape;
	b2PolygonShape boxShape;

	boxShape.SetAsBox(
		PIXELS_TO_METERS * m_Attributes.boxSize.x * m_Attributes.scale.x * 0.5f,
		PIXELS_TO_METERS * m_Attributes.boxSize.y * m_Attributes.scale.y * 0.5f,
		b2Vec2{m_Attributes.offset.x * PIXELS_TO_METERS,
			   m_Attributes.offset.y * PIXELS_TO_METERS},
		0.0f);

	b2FixtureDef fixtureDef{};
	fixtureDef.shape = &boxShape;

	fixtureDef.density = m_Attributes.density;
	fixtureDef.friction = m_Attributes.friction;
	fixtureDef.restitution = m_Attributes.restitution;
	fixtureDef.restitutionThreshold = m_Attributes.restitutionThreshold;
	fixtureDef.isSensor = m_Attributes.isSensor;

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

	lua.new_usertype<Blazr::PhysicsAttributes>(
		"PhysicsAttributes", sol::call_constructor,
		sol::factories(
			[] { return Blazr::PhysicsAttributes{}; },
			[](const sol::table phyAttr) {
				return Blazr::PhysicsAttributes{
					.type =
						phyAttr["type"].get_or(Blazr::RigidBodyType::STATIC),
					.density = phyAttr["density"].get_or(100.f),
					.friction = phyAttr["friction"].get_or(0.2f),
					.restitution = phyAttr["restitution"].get_or(0.2f),
					.restitutionThreshold =
						phyAttr["restitutionThreshold"].get_or(0.2f),
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
					.filterCategory =
						phyAttr["filterCategory"].get_or((uint16_t)0),
					.filterMask = phyAttr["filterMask"].get_or((uint16_t)0),
					.filterGroup = phyAttr["filterGroup"].get_or((int16_t)0)};
			}),
		"type", &Blazr::PhysicsAttributes::type, "density",
		&Blazr::PhysicsAttributes::density, "friction",
		&Blazr::PhysicsAttributes::friction, "restitution",
		&Blazr::PhysicsAttributes::restitution, "restitutionThreshold",
		&Blazr::PhysicsAttributes::restitutionThreshold, "radius",
		&Blazr::PhysicsAttributes::gravityScale, "position",
		&Blazr::PhysicsAttributes::position, "scale",
		&Blazr::PhysicsAttributes::scale, "boxSize",
		&Blazr::PhysicsAttributes::boxSize, "offset",
		&Blazr::PhysicsAttributes::offset, "isSensor",
		&Blazr::PhysicsAttributes::isSensor, "isFixedRotation",
		&Blazr::PhysicsAttributes::isFixedRotation, "isCircle",
		&Blazr::PhysicsAttributes::filterCategory, "filterMask",
		&Blazr::PhysicsAttributes::filterMask, "filterGroup",
		&Blazr::PhysicsAttributes::filterGroup, "set_transform",
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

			body->SetTransform(b2Vec2{bx, by}, 0.f);
		});

	auto &physicsWorld = registry.GetContext<std::shared_ptr<b2World>>();

	if (!physicsWorld) {
		return;
	}
	lua.new_usertype<Blazr::PhysicsComponent>(
		"PhysicsComponent", "type_id",
		&entt::type_hash<Blazr::PhysicsComponent>::value, sol::call_constructor,
		sol::factories([&](const Blazr::PhysicsAttributes &attr) {
			Blazr::PhysicsComponent pc{physicsWorld, attr};
			pc.init(1280, 720); // TODO: Change based on window values
			return pc;
		}),

		"linear_impulse",
		[](PhysicsComponent &pc, const glm::vec2 &impulse) {
			auto body = pc.GetRigidBody();
			if (!body) {
				// TODO: Add Error
				return;
			}

			body->ApplyLinearImpulse(b2Vec2{impulse.x, impulse.y},
									 body->GetPosition(), true);
		},
		"angular_impulse",
		[](PhysicsComponent &pc, float impulse) {
			auto body = pc.GetRigidBody();
			if (!body) {
				// TODO: Add Error
				return;
			}

			body->ApplyAngularImpulse(impulse, true);
		},
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
		"get_angular_velocity",
		[](PhysicsComponent &pc) {
			auto body = pc.GetRigidBody();
			if (!body) {
				// TODO: Add Error
				return 0.f;
			}

			return body->GetAngularVelocity();
		},
		"set_gravity_scale",
		[](PhysicsComponent &pc, float gravityScale) {
			auto body = pc.GetRigidBody();
			if (!body) {
				// TODO: Add Error
				return;
			}

			body->SetGravityScale(gravityScale);
		},
		"get_gravity_scale",
		[](PhysicsComponent &pc) {
			auto body = pc.GetRigidBody();
			if (!body) {
				// TODO: Add Error
				return 0.f;
			}

			return body->GetGravityScale();
		},
		"set_transform",
		[](PhysicsComponent &pc, const glm::vec2 &position) {
			auto body = pc.GetRigidBody();
			if (!body) {
				// TODO: Add Error
				return;
			}

			const auto scaleHalfHeight = 1280 * PIXELS_TO_METERS * 0.5f;
			const auto scaleHalfWidth = 720 * PIXELS_TO_METERS * 0.5f;

			auto bx = (position.x * PIXELS_TO_METERS) - scaleHalfWidth;
			auto by = (position.y * PIXELS_TO_METERS) - scaleHalfHeight;

			body->SetTransform(b2Vec2{bx, by}, 0.f);
		},
		"get_transform",
		[](const PhysicsComponent &pc) {

		},
		"set_body_type",
		[&](PhysicsComponent &pc, RigidBodyType type) {
			auto body = pc.GetRigidBody();
			if (!body) {
				// TODO: Add Error
				return;
			}

			b2BodyType bodyType = b2_dynamicBody;

			switch (type) {
			case RigidBodyType::STATIC:
				bodyType = b2_staticBody;
				break;
			case RigidBodyType::DYNAMIC:
				bodyType = b2_dynamicBody;
				break;
			case RigidBodyType::KINEMATIC:
				bodyType = b2_kinematicBody;
				break;
			default:
				break;
			}

			body->SetType(bodyType);
		},
		"set_bullet",
		[&](PhysicsComponent &pc, bool bullet) {
			auto body = pc.GetRigidBody();
			if (!body) {
				// TODO: Add Error
				return;
			}
			body->SetBullet(bullet);
		},
		"is_bullet",
		[&](PhysicsComponent &pc) {
			auto body = pc.GetRigidBody();
			if (!body) {
				// TODO: Add Error
				return false;
			}
			return body->IsBullet();
		});
}

void Blazr::PhysicsComponent::to_json(nlohmann::json &j,
									  PhysicsComponent component) {
	const auto &attributes = component.m_Attributes;

	j = nlohmann::json{
		{"type", static_cast<int>(attributes.type)},
		{"density", attributes.density},
		{"friction", attributes.friction},
		{"restitution", attributes.restitution},
		{"restitutionThreshold", attributes.restitutionThreshold},
		{"gravityScale", attributes.gravityScale},
		{"position", {attributes.position.x, attributes.position.y}},
		{"scale", {attributes.scale.x, attributes.scale.y}},
		{"boxSize", {attributes.boxSize.x, attributes.boxSize.y}},
		{"offset", {attributes.offset.x, attributes.offset.y}},
		{"isSensor", attributes.isSensor},
		{"isFixedRotation", attributes.isFixedRotation},
		{"filterCategory", attributes.filterCategory},
		{"filterMask", attributes.filterMask},
		{"filterGroup", attributes.filterGroup}};
}

void Blazr::PhysicsComponent::from_json(const nlohmann::json &j,
										PhysicsComponent &component) {
	auto &attributes = component.m_Attributes;

	attributes.type = static_cast<RigidBodyType>(j.at("type").get<int>());
	attributes.density = j.at("density").get<float>();
	attributes.friction = j.at("friction").get<float>();
	attributes.restitution = j.at("restitution").get<float>();
	attributes.restitutionThreshold = j.at("restitutionThreshold").get<float>();
	attributes.gravityScale = j.at("gravityScale").get<float>();

	auto position = j.at("position").get<std::vector<float>>();
	attributes.position = glm::vec2(position[0], position[1]);

	auto scale = j.at("scale").get<std::vector<float>>();
	attributes.scale = glm::vec2(scale[0], scale[1]);

	auto boxSize = j.at("boxSize").get<std::vector<float>>();
	attributes.boxSize = glm::vec2(boxSize[0], boxSize[1]);

	auto offset = j.at("offset").get<std::vector<float>>();
	attributes.offset = glm::vec2(offset[0], offset[1]);

	attributes.isSensor = j.at("isSensor").get<bool>();
	attributes.isFixedRotation = j.at("isFixedRotation").get<bool>();

	attributes.filterCategory = j.at("filterCategory").get<uint16_t>();
	attributes.filterMask = j.at("filterMask").get<uint16_t>();
	attributes.filterGroup = j.at("filterGroup").get<int16_t>();
}
