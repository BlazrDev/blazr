#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
namespace Blazr {
class RendererAPI {

  public:
	bool Init();

	void SetClearColor(float r, float g, float b, float a);

	void Clear();

	void SwapBuffers();

	void PollEvents();

	GLFWwindow *getWindow() const { return window; }

	// void DrawRectangle(float x, float y, float width, float height,
	// 				   const glm::vec4 &color = {1.0f, 1.0f, 1.0f, 1.0f});
	void DrawRectangle(float x, float y, float width, float height,
					   const glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f});

	bool WindowShouldClose() const;

	void Shutdown();

  private:
	GLFWwindow *window = nullptr;
};

} // namespace Blazr
