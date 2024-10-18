#include "blzrpch.h"
#include "Camera2D.h"

namespace Blazr {

Camera2D::Camera2D(float width, float height)
	: width(width), height(height), position(0.0f, 0.0f), zoom(1.0f) {}

void Camera2D::SetPosition(const glm::vec2 &pos) { position = pos; }

glm::vec2 Camera2D::GetPosition() const { return position; }

void Camera2D::SetZoom(float z) { zoom = z; }

float Camera2D::GetZoom() const { return zoom; }

glm::mat4 Camera2D::GetViewMatrix() const {
	return glm::translate(glm::mat4(1.0f), glm::vec3(-position, 0.0f)) *
		   glm::scale(glm::mat4(1.0f), glm::vec3(zoom, zoom, 1.0f));
}

glm::mat4 Camera2D::GetProjectionMatrix() const {
	return glm::ortho(0.0f, width, height, 0.0f);
}

} // namespace Blazr
