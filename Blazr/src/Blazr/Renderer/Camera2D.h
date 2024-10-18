#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

namespace Blazr {

class Camera2D {
  public:
	Camera2D(float width, float height);

	void SetPosition(const glm::vec2 &position);

	glm::vec2 GetPosition() const;

	void SetZoom(float zoom);

	float GetZoom() const;

	glm::mat4 GetViewMatrix() const;

	glm::mat4 GetProjectionMatrix() const;

  private:
	glm::vec2 position;
	float zoom;
	float width, height;
};

} // namespace Blazr
