#pragma once
#include <glm.hpp>

namespace Blazr {

struct Vertex {
	glm::vec3 Position;
	glm::vec4 Color;

	Vertex() = default;

	Vertex(const glm::vec3 &position, const glm::vec4 &color)
		: Position(position), Color(color) {}
};

} // namespace Blazr
