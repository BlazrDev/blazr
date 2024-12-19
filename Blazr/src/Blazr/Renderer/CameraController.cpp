#include "blzrpch.h"
#include "GLFW/glfw3.h"
#include "Blazr/Core/KeyCodes.h"
#include "Blazr/Core/Log.h"
#include "Blazr/Events/ApplicationEvent.h"
#include "Blazr/Events/MouseEvent.h"
#include "CameraController.h"

bool Blazr::CameraController::paused = false;
bool Blazr::CameraController::gameViewWindow = false;
Blazr::CameraController::CameraController(int width, int height, bool rotation)
	: m_Width(width), m_Height(height), m_Rotation(rotation) {
	m_Camera = *Camera2D::GetInstance();
	BLZR_CORE_INFO("CameraController initialized!");
}
void Blazr::CameraController::OnUpdate() {
	if (!paused)
		return;
	if (!gameViewWindow)
		return;

	int ts = 30;
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_A) == GLFW_PRESS) {
		m_CameraPosition.x -=
			cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
		m_CameraPosition.y -=
			sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
	} else if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_D) == GLFW_PRESS) {
		m_CameraPosition.x +=
			cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
		m_CameraPosition.y +=
			sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
	}

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_W) == GLFW_PRESS) {
		m_CameraPosition.x += -sin(glm::radians(m_CameraRotation)) *
							  m_CameraTranslationSpeed * ts;
		m_CameraPosition.y +=
			cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
	} else if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_S) == GLFW_PRESS) {
		m_CameraPosition.x -= -sin(glm::radians(m_CameraRotation)) *
							  m_CameraTranslationSpeed * ts;
		m_CameraPosition.y -=
			cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
	}

	if (m_Rotation) {
		if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_Q) == GLFW_PRESS)
			m_CameraRotation += 0.5f;
		// m_CameraRotation += m_CameraRotationSpeed * ts;
		if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_E) == GLFW_PRESS)
			m_CameraRotation -= 0.5f;

		// m_CameraRotation -= m_CameraRotationSpeed * ts;

		if (m_CameraRotation > 180.0f)
			m_CameraRotation -= 360.0f;
		else if (m_CameraRotation <= -180.0f)
			m_CameraRotation += 360.0f;

		m_Camera.SetRotation(m_CameraRotation);
	}

	m_Camera.SetPosition(m_CameraPosition);

	m_CameraTranslationSpeed = 0.25;
}
void Blazr::CameraController::OnEvent(Event &e) {
	if (!paused)
		return;
	if (!gameViewWindow)
		return;

	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<MouseScrolledEvent>(
		[this](MouseScrolledEvent &event) { return OnMouseScrolled(event); });

	dispatcher.Dispatch<WindowResizeEvent>(
		[this](WindowResizeEvent &event) { return OnWindowResized(event); });

	dispatcher.Dispatch<MouseMovedEvent>(
		[this](MouseMovedEvent &event) { return OnMouseMove(event); });
}

void Blazr::CameraController::OnResize(float width, float height) {
	m_Camera.SetProjection(0.f, width, 0.f, height);
}

bool Blazr::CameraController::OnMouseScrolled(MouseScrolledEvent &e) {
	m_Zoom -= e.getYOffset() * 0.25f;
	m_Zoom = std::max(m_Zoom, 0.25f);
	m_Camera.SetScale(m_Zoom);

	return false;
}

bool Blazr::CameraController::OnWindowResized(WindowResizeEvent &e) {
	OnResize((float)e.getWidth(), (float)e.getHeight());
	return false;
}

bool Blazr::CameraController::OnMouseMove(MouseMovedEvent &e) {
	float mouseX = e.getX() - 285;
	float mouseY = e.getY() - 89;
	if (mouseY < 0)
		mouseY = 0;
	if (mouseX < 0)
		mouseX = 0;
	m_MousePosition = {mouseX, mouseY};
	return false;
}

void Blazr::CameraController::to_json(nlohmann::json &j,
									  CameraController &controller) {
	nlohmann::json cameraJson;
	Camera2D::to_json(cameraJson, controller.m_Camera);

	j = nlohmann::json{
		{"width", controller.m_Width},
		{"height", controller.m_Height},
		{"camera_position",
		 {controller.m_CameraPosition.x, controller.m_CameraPosition.y,
		  controller.m_CameraPosition.z}},
		{"camera_rotation", controller.m_CameraRotation},
		{"camera_translation_speed", controller.m_CameraTranslationSpeed},
		{"camera_rotation_speed", controller.m_CameraRotationSpeed},
		{"rotation", controller.m_Rotation},
		{"zoom", controller.m_Zoom},
		{"mouse_position",
		 {controller.m_MousePosition.x, controller.m_MousePosition.y}},
		{"camera", cameraJson}};
}

void Blazr::CameraController::from_json(nlohmann::json &j,
										CameraController &controller) {
	j.at("width").get_to(controller.m_Width);
	j.at("height").get_to(controller.m_Height);

	auto position = j.at("camera_position").get<std::vector<float>>();
	controller.m_CameraPosition = {position[0], position[1], position[2]};

	j.at("camera_rotation").get_to(controller.m_CameraRotation);
	j.at("camera_translation_speed")
		.get_to(controller.m_CameraTranslationSpeed);
	j.at("camera_rotation_speed").get_to(controller.m_CameraRotationSpeed);
	j.at("rotation").get_to(controller.m_Rotation);
	j.at("zoom").get_to(controller.m_Zoom);

	auto mouse_pos = j.at("mouse_position").get<std::vector<float>>();
	controller.m_MousePosition = {mouse_pos[0], mouse_pos[1]};

	nlohmann::json camera_json = j.at("camera");
	Camera2D::from_json(camera_json, controller.m_Camera);
}
