#include "blzrpch.h"
#include "FollowCamera.h"

namespace Blazr {
FollowCamera::FollowCamera(Camera2D &camera, Ref<Entity> target)
	: m_Camera(camera), m_Target(target) {}

void FollowCamera::Update() {
	if (!m_Target->HasComponent<TransformComponent>()) {
		return;
	}

	auto &transform = m_Target->GetComponent<TransformComponent>();
	float cameraWidth = m_Camera.GetWidth();
	float cameraHeight = m_Camera.GetHeight();
	auto cameraPosition = m_Camera.GetPosition();
	int cameraScale = m_Camera.GetScale();

	glm::vec2 newCamPosition{0.f};

	newCamPosition.x =
		(transform.position.x - cameraWidth / (2 * cameraScale)) * cameraScale;

	newCamPosition.y =
		(transform.position.y - cameraHeight / (2 * cameraScale)) * cameraScale;

	newCamPosition.x = std::clamp(newCamPosition.x, 0.f, cameraWidth);
	// cameraWidth - cameraWidth / cameraScale);

	newCamPosition.y = std::clamp(newCamPosition.y, 0.f, cameraHeight);
	// cameraHeight - cameraHeight / cameraScale);

	// newCamPosition.x = transform.position.x - cameraWidth / 2;
	// newCamPosition.y = transform.position.y - cameraHeight / 2;
	m_Camera.SetPosition(newCamPosition);
}
} // namespace Blazr
