#pragma once

#include "Blazr/Core/Core.h"
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

	static void DrawQuad(int entityID, const glm::vec2 &position,
						 const glm::vec2 &size, const glm::vec4 &color);

	static void DrawQuad(int entityID, const glm::vec2 &position,
						 const glm::vec2 &size, const Ref<Texture2D> &texture,
						 float tilingFactor = 1.0f,
						 const glm::vec4 &tintColor = glm::vec4(1.0f));
	// static void DrawQuad(const glm::mat4 &transform, const glm::vec4 &color,
	// 					 int entityID = -1);
	// static void DrawQuad(const glm::mat4 &transform,
	// 					 const Ref<Texture2D> &texture,
	// 					 float tilingFactor = 1.0f,
	// 					 const glm::vec4 tintColor = glm::vec4(1.0f),
	// 					 int entityID = -1);

  private:
	static void StartBatch();
	static void NextBatch();
};
} // namespace Blazr