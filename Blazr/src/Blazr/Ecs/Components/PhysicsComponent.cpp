#include "Blazr/Core/Log.h"
#include "Blazr/Physics/Box2DWrapper.h"
#include "PhysicsComponent.h"
#include "box2d/box2d.h"
#include "box2d/collision.h"
#include "box2d/id.h"

Blazr::PhysicsComponent::PhysicsComponent(Blazr::PhysicsWorld world,
										  const PhysicsAtributes &atributes)
	: m_World(world), m_Atributes(atributes) {}

void Blazr::PhysicsComponent::init(int windowWidth, int windowHeight) {
	// if (!m_World) {
	// 	BLZR_CORE_ERROR("PhysicsComponent::init: PhysicsWorld is nullptr");
	// 	return;
	// }

	b2BodyDef bodyDef = b2DefaultBodyDef();
	bodyDef.type = static_cast<b2BodyType>(m_Atributes.type);
	// bodyDef.type = b2BodyType::b2_dynamicBody;

	bodyDef.position = b2Vec2{
		m_Atributes.position.x + m_Atributes.offset.x - (windowWidth / 2.f) +
			m_Atributes.boxSize.x * m_Atributes.scale.x / 2.f *
				PIXELS_TO_METERS,
		m_Atributes.position.y + m_Atributes.offset.y - (windowHeight / 2.f) +
			m_Atributes.boxSize.y * m_Atributes.scale.y / 2.f *
				PIXELS_TO_METERS};

	bodyDef.position = b2Vec2{0.0f, 4.0f};
	bodyDef.gravityScale = m_Atributes.gravityScale;
	bodyDef.fixedRotation = m_Atributes.isFixedRotation;

	m_RigidBody = b2CreateBody(m_World, &bodyDef);

	// if (!m_RigidBody) {
	// 	BLZR_CORE_ERROR("PhysicsComponent::init: Failed to create rigid body");
	// 	return;
	// }

	b2Polygon box = b2MakeBox(
		m_Atributes.boxSize.x * m_Atributes.scale.x / 2.f * PIXELS_TO_METERS,
		m_Atributes.boxSize.y * m_Atributes.scale.y / 2.f * PIXELS_TO_METERS);

	// b2Polygon box = b2MakeBox(1.0f, 1.0f);
	b2ShapeDef shapeDef = b2DefaultShapeDef();
	shapeDef.density = m_Atributes.density;
	shapeDef.friction = m_Atributes.friction;
	shapeDef.restitution = m_Atributes.restitution;
	shapeDef.isSensor = m_Atributes.isSensor;

	b2ShapeId shapeId = b2CreatePolygonShape(m_RigidBody, &shapeDef, &box);
}
static void CreateLuaPhysicsComponentBind(sol::state_view &lua,
										  Blazr::PhysicsWorld world);
