#pragma once

#include "blzrpch.h"
#include "Blazr/Events/ApplicationEvent.h"
#include "Blazr/Events/Event.h"
#include "Blazr/Events/MouseEvent.h"
#include "Camera2D.h"

namespace Blazr {
class CameraController {
  public:
	static bool BLZR_API paused;
	static bool BLZR_API gameViewWindow;

	CameraController(int width, int height, bool rotation = true);
	void OnUpdate();
	void OnEvent(Event &e);
	Camera2D &GetCamera() { return m_Camera; }

	glm::vec2 &GetMousePosition() { return m_MousePosition; }

	bool OnMouseScrolled(MouseScrolledEvent &e);
	bool OnWindowResized(WindowResizeEvent &e);
	bool OnMouseMove(MouseMovedEvent &e);

  private:
	void OnResize(float width, float height);
	Camera2D m_Camera;
	int m_Width, m_Height;
	glm::vec3 m_CameraPosition = {0.0f, 0.0f, 0.0f};
	float m_CameraRotation = 0.0f;
	float m_CameraTranslationSpeed = 5.0f, m_CameraRotationSpeed = 180.0f;
	bool m_Rotation;
	float m_Zoom{1.0f};
	glm::vec2 m_MousePosition = {0.0f, 0.0f};
};
} // namespace Blazr
