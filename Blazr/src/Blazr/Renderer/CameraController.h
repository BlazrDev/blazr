#pragma once

#include "blzrpch.h"
#include "Blazr/Events/ApplicationEvent.h"
#include "Blazr/Events/Event.h"
#include "Blazr/Events/MouseEvent.h"
#include "Camera2D.h"

namespace Blazr {
class CameraController {
  public:
	static bool paused;

	CameraController(int width, int height, bool rotation = false);
	void OnUpdate();
	void OnEvent(Event &e);
	Camera2D &GetCamera() { return m_Camera; }

  private:
	void OnResize(float width, float height);
	bool OnMouseScrolled(MouseScrolledEvent &e);
	bool OnWindowResized(WindowResizeEvent &e);
	Camera2D m_Camera;
	int m_Width, m_Height;
	glm::vec3 m_CameraPosition = {0.0f, 0.0f, 0.0f};
	float m_CameraRotation = 0.0f;
	float m_CameraTranslationSpeed = 5.0f, m_CameraRotationSpeed = 180.0f;
	bool m_Rotation;
	float m_Zoom{1.0f};
};
} // namespace Blazr
