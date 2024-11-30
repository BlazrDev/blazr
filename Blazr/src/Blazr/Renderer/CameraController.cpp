#include "GLFW/glfw3.h"
#include "Blazr/Core/KeyCodes.h"
#include "Blazr/Core/Log.h"
#include "Blazr/Events/ApplicationEvent.h"
#include "Blazr/Events/MouseEvent.h"
#include "CameraController.h"

bool Blazr::CameraController::paused = false;
Blazr::CameraController::CameraController(int width, int height, bool rotation)
	: m_Width(width), m_Height(height), m_Rotation(rotation) {
	m_Camera = *Camera2D::GetInstance();
	BLZR_CORE_INFO("CameraController initialized!");
}
void Blazr::CameraController::OnUpdate() {
	if (!paused)
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
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<MouseScrolledEvent>(
		[this](MouseScrolledEvent &event) { return OnMouseScrolled(event); });

	dispatcher.Dispatch<WindowResizeEvent>(
		[this](WindowResizeEvent &event) { return OnWindowResized(event); });
}

void Blazr::CameraController::OnResize(float width, float height) {
	float scaledWidth = width * 0.5f;
	float scaledHeight = height * 0.5f;
	m_Camera.SetProjection(-width * m_Zoom, width * m_Zoom, -height * m_Zoom,
						   height * m_Zoom);
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
