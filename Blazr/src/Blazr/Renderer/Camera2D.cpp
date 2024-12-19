#include "blzrpch.h"
#include "Camera2D.h"

namespace Blazr {

Camera2D::Camera2D() : Camera2D(1280, 720){};

Ref<Blazr::Camera2D> Blazr::Camera2D::instance = nullptr;
Camera2D::Camera2D(int width, int height)
	: m_Width(width), m_Height(height), m_Scale(1.f), m_Position(glm::vec2{0}),
	  m_CameraMatrix{1.f}, m_OrthoProjection{1.f}, m_bNeedsUpdate(true) {
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
void Camera2D::SetProjection(float left, float right, float bottom, float top) {
	m_OrthoProjection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
	m_CameraMatrix = m_OrthoProjection;
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

	glm::vec3 rotationAxis(0.f, 0.f, 1.f); // Rotacija oko Z ose
	m_CameraMatrix =
		glm::rotate(m_CameraMatrix, glm::radians(m_Rotation), rotationAxis);

	glm::vec3 scale{m_Scale, m_Scale, 0.f};
	m_CameraMatrix *= glm::scale(glm::mat4(1.f), scale);

	m_bNeedsUpdate = false;
}

void Camera2D::to_json(nlohmann::json &j, const Camera2D &camera) {
	j = nlohmann::json{
		{"position", {camera.GetPosition().x, camera.GetPosition().y}},
		{"scale", camera.GetScale()},
		{"rotation", camera.GetRotation()},
		{"width", camera.GetWidth()},
		{"height", camera.GetHeight()}};
}

void Camera2D::from_json(const nlohmann::json &j, Camera2D &camera) {
	auto position = j.at("position").get<std::vector<float>>();
	camera.SetPosition({position[0], position[1]});

	float scale = j.at("scale").get<float>();
	camera.SetScale(scale);

	float rotation = j.at("rotation").get<float>();
	camera.SetRotation(rotation);

	// Width and height are set via constructor and might not be mutable
	// directly. Ensure your Camera2D class allows updating width and height if
	// needed.
	int width = j.at("width").get<int>();
	int height = j.at("height").get<int>();

	if (camera.GetWidth() != width || camera.GetHeight() != height) {
		camera = Camera2D(
			width,
			height); // Create a new camera instance with the updated dimensions
	}
}

} // namespace Blazr
