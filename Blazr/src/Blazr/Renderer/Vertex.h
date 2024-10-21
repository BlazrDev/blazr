#pragma once
#include "../../../vendor/GLFW/include/GLFW/glfw3.h"
#include <glm.hpp>

namespace Blazr {

struct Color {
	GLubyte r, g, b, a;
};

struct Vertex {
	glm::vec2 position{0.f}, uvs{0.f};
	Color color{.r = 255, .g = 0, .b = 255, .a = 255};

	void set_color(GLubyte r, GLubyte g, GLubyte b, GLubyte a) {
		color.r = r;
		color.g = g;
		color.b = b;
		color.a = a;
	}

	void set_color(GLuint colors) {
		color.r = 0xff & (colors >> 24);
		color.g = 0xff & (colors >> 16);
		color.b = 0xff & (colors >> 8);
		color.a = 0xff & colors;
	}
	Vertex() = default;

	Vertex(const glm::vec2 &position, const glm::vec2 &uvs, Color color)
		: position(position), color(color), uvs(uvs) {}
};

} // namespace Blazr
