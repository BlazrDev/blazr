#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "Blazr/Core/Core.h"

namespace Blazr {

class Camera2D {
  public:
	BLZR_API Camera2D();
	BLZR_API Camera2D(int width, int height);

	void SetPosition(const glm::vec2 &position);

	glm::vec2 GetPosition() const;

	void BLZR_API SetScale(const float &);

	float BLZR_API GetScale() const;

	void BLZR_API Update();

	glm::mat4 GetCameraMatrix() const;

	glm::mat4 GetOrthoProjection() const;

	static Ref<Camera2D> BLZR_API &GetInstance() {
		if (instance == nullptr) {
			instance = std::make_shared<Camera2D>();
		}

		return instance;
	}

  private:
	glm::vec2 m_Position;
	glm::mat4 m_CameraMatrix, m_OrthoProjection;

	float m_Scale;
	int m_Width, m_Height;

	bool m_bNeedsUpdate;

	static Ref<Camera2D> instance;
};

} // namespace Blazr
