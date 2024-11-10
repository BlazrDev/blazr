#pragma once
#include "Blazr/Core/Core.h"
#include "glm.hpp"
#include <gtc/matrix_transform.hpp>

namespace Blazr {

class Camera2D {
  public:
	Camera2D();
	Camera2D(int width, int height);

	void BLZR_API SetPosition(const glm::vec2 &position);

	glm::vec2 BLZR_API GetPosition() const;

	void BLZR_API SetScale(const float &);

	float BLZR_API GetScale() const;

	void Update();

	glm::mat4 GetCameraMatrix() const;

	glm::mat4 GetOrthoProjection() const;

  private:
	glm::vec2 m_Position;
	glm::mat4 m_CameraMatrix, m_OrthoProjection;

	float m_Scale;
	int m_Width, m_Height;

	bool m_bNeedsUpdate;
};

} // namespace Blazr
