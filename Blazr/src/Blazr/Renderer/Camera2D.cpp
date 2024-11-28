#include "blzrpch.h"
#include "Camera2D.h"

namespace Blazr {

Camera2D::Camera2D() : Camera2D(1280, 720){};

Ref<Blazr::Camera2D> Blazr::Camera2D::instance = nullptr;
Camera2D::Camera2D(int width, int height)
	: m_Width(width), m_Height(height), m_Scale(1.f), m_Position(glm::vec2{0}),
	  m_CameraMatrix{1.f}, m_OrthoProjection{1.f}, m_bNeedsUpdate(true) {
	// m_OrthoProjection = glm::ortho(-static_cast<float>(m_Width / 2),  // Left
	// 							   static_cast<float>(m_Width / 2),	  // Right
	// 							   -static_cast<float>(m_Height / 2), // Bottom
	// 							   static_cast<float>(m_Height / 2),  // Top
	// 							   -1.f,							  // Near
	// 							   1.f);
	m_OrthoProjection = glm::ortho(0.0f,						 // Left
								   static_cast<float>(m_Width),	 // Right
								   0.0f,						 // Bottom
								   static_cast<float>(m_Height), // Top
								   -1.f,						 // Near
								   1.f);
}

void Camera2D::SetPosition(const glm::vec2 &pos) {
	m_Position = pos;
	m_bNeedsUpdate = true;
}
void Camera2D::SetScale(const float &scale) {
	m_Scale = scale;
	m_bNeedsUpdate = true;
}

float Camera2D::GetScale() const { return m_Scale; }

glm::vec2 Camera2D::GetPosition() const { return m_Position; }

glm::mat4 Camera2D::GetCameraMatrix() const { return m_CameraMatrix; }

glm::mat4 Camera2D::GetOrthoProjection() const { return m_OrthoProjection; }

void Camera2D::Update() {
	if (!m_bNeedsUpdate) {
		return;
	}

	glm::vec3 translate(-m_Position.x, -m_Position.y, 0.f);
	m_CameraMatrix = glm::translate(m_OrthoProjection, translate);

	glm::vec3 scale{m_Scale, m_Scale, 0.f};
	m_CameraMatrix *= glm::scale(glm::mat4(1.f), scale);

	m_bNeedsUpdate = false;
}

} // namespace Blazr
