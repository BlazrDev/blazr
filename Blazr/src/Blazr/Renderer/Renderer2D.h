#pragma once

#include "Blazr/Core/Core.h"
#include "Blazr/Ecs/Components/SpriteComponent.h"
#include "Blazr/Ecs/Components/TransformComponent.h"
#include "Blazr/Ecs/Registry.h"
#include "Blazr/Fonts/Font.h"
#include "Camera2D.h"
#include "Texture2D.h"

namespace Blazr {

class Renderer2D {
  public:
	static void Init();
	static void Shutdown();

	static void BeginScene(const Camera2D &camera);
	static void EndScene();
	static void Flush();

	static void DrawQuad(entt::entity entityID, const glm::vec2 &position,
						 const glm::vec2 &size, const glm::vec4 &color);

	static void DrawQuad(entt::entity entityID, const glm::vec2 &position,
						 const glm::vec2 &size, const Ref<Texture2D> &texture,
						 float tilingFactor = 1.0f,
						 const glm::vec4 &tintColor = glm::vec4(1.0f));
	// static void DrawQuad(const glm::mat4 &transform, const glm::vec4 &color,
	// 					 int entityID = -1);
	// static void DrawQuad(const glm::mat4 &transform,
	// 					 const Ref<Texture2D> &texture,
	// 					 float tilingFactor = 1.0f,
	// 					 const glm::vec4 tintColor = glm::vec4(1.0f),
	// 					 int entityID = -1)
	static void DrawQuad(Registry &registry, entt::entity entityID);
	static void DrawQuad(entt::entity entityID, const glm::vec2 &position,
						 const glm::vec2 &size, float rotation,
						 const glm::vec2 &scale, const Ref<Texture2D> &texture,
						 float tilingFactor = 1.0f,
						 const glm::vec4 &tintColor = glm::vec4(1.0f));
	static void DrawQuad(entt::entity entityID, const glm::vec2 &position,
						 const glm::vec2 &size, const Ref<Texture2D> &texture,
						 float rotation, const glm::vec2 &scale,
						 float tilingFactor, const glm::vec4 &tintColor,
						 const glm::vec2 *textureCoords);

	// draw texture using UVs
	static void DrawQuad(const glm::vec2 &position, const glm::vec2 &size,
						 const Ref<Texture2D> &texture,
						 const glm::vec2 uvCoords[4], const glm::vec4 &color);

	// draw text
	static void DrawText(const std::string &text, const glm::vec2 &position,
						 const glm::vec4 &color, const Ref<Font> &font,
						 float scale = 1.0f, float maxWidth = 10000);

	static void Clear();

  private:
	static void StartBatch();
	static void NextBatch();
	static void RenderCharacter(const Glyph &glyph, const glm::vec2 &position,
								const glm::vec4 &color, float scale,
								const Ref<Texture2D> &textureAtlas);
};
} // namespace Blazr
