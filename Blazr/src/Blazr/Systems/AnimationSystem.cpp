#include "blzrpch.h"
#include "GLFW/glfw3.h"
#include "AnimationSystem.h"
#include "Blazr/Ecs/Components/AnimationComponent.h"
#include "Blazr/Ecs/Components/SpriteComponent.h"
#include "Blazr/Ecs/Components/TransformComponent.h"

Blazr::AnimationSystem::AnimationSystem(Registry &registry)
	: m_Registry(registry) {}
void Blazr::AnimationSystem::Update() {
	auto view =
		m_Registry.GetRegistry()
			.view<AnimationComponent, SpriteComponent, TransformComponent>();

	for (auto entity : view) {
		auto &animation = view.get<AnimationComponent>(entity);
		auto &sprite = view.get<SpriteComponent>(entity);

		animation.currentFrame =
			(int)(glfwGetTime() * animation.frameRate) % animation.numFrames;

		if (animation.bVertical) {
			sprite.object.x = animation.currentFrame;
			sprite.object.y = animation.frameOffset;

		} else {
			sprite.object.coordX = animation.currentFrame;
			sprite.object.coordY = animation.frameOffset;
		}

		// sprite.generateTextureCoordinates();
	}
}
