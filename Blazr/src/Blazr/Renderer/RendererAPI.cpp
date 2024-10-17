#include "RendererAPI.h"
#include "ext/vector_float3.hpp"
#include <iostream>

namespace Blazr {
bool RendererAPI::Init() {
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return false;
	}

	window = glfwCreateWindow(800, 600, "OpenGL Renderer", nullptr, nullptr);
	if (!window) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return false;
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return true;
}

void RendererAPI::SetClearColor(float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
}

void RendererAPI::Clear() { glClear(GL_COLOR_BUFFER_BIT); }

void RendererAPI::SwapBuffers() { glfwSwapBuffers(window); }

void RendererAPI::PollEvents() { glfwPollEvents(); }

void RendererAPI::DrawRectangle(float x, float y, float width, float height,
								const glm::vec4 color) {
	glBegin(GL_QUADS);
	glColor4f(color.r, color.g, color.b, color.a);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glVertex2f(x, y);
	glVertex2f(x + width, y);
	glVertex2f(x + width, y + height);
	glVertex2f(x, y + height);
	glEnd();
}

bool RendererAPI::WindowShouldClose() const {
	return glfwWindowShouldClose(window);
}

void RendererAPI::Shutdown() {
	glfwDestroyWindow(window);
	glfwTerminate();
}
} // namespace Blazr
