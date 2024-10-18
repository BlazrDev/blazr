#pragma once
#include "Buffer.h"
#include "Camera2D.h"
#include "Vertex.h"
#include "VertexArray.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <vector>

namespace Blazr {

class RendererAPI {
  public:
	bool Init();

	void SetClearColor(float r, float g, float b, float a);
	void Clear();

	void BeginBatch();
	void EndBatch();
	void Flush();

	void SwapBuffers();

	void PollEvents();

	void DrawRectangle(float x, float y, float width, float height,
					   const glm::vec4 color, const Camera2D &camera);

	bool WindowShouldClose() const;

	GLFWwindow *getWindow() const { return window; }
	void Shutdown();

  private:
	void AddRectangleToBatch(float x, float y, float width, float height,
							 const glm::vec4 color);

  private:
	static const uint32_t MaxQuads = 1000;
	static const uint32_t MaxVertices = MaxQuads * 4;
	static const uint32_t MaxIndices = MaxQuads * 6;

	std::vector<Vertex> m_VertexBufferData;
	uint32_t m_IndexCount = 0;

	Ref<VertexArray> m_VertexArray;
	Ref<VertexBuffer> m_VertexBuffer;
	Ref<IndexBuffer> m_IndexBuffer;

	GLFWwindow *window = nullptr;
};

} // namespace Blazr
