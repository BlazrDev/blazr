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

	newCamPosition.y = std::clamp(newCamPosition.y, 0.f, cameraHeight);

	// newCamPosition.x = transform.position.x - cameraWidth / 2;
	// newCamPosition.y = transform.position.y - cameraHeight / 2;
	m_Camera.SetPosition(newCamPosition);
}
void FollowCamera::to_json(nlohmann::json &j,
						   const FollowCamera &followCamera) {
	j["Position"] = {followCamera.m_Position.x, followCamera.m_Position.y};
	if (followCamera.m_Target &&
		followCamera.m_Target->HasComponent<Identification>()) {
		nlohmann::json targetJson;
		Identification::to_json(
			targetJson, followCamera.m_Target->GetComponent<Identification>());
		j["Target"] = targetJson;
	} else {
		j["Target"] = nullptr;
	}
}

// Deserialization
void FollowCamera::from_json(const nlohmann::json &j,
							 FollowCamera &followCamera, Camera2D &camera) {
	if (j.contains("Position")) {
		followCamera.m_Position.x = j["Position"][0];
		followCamera.m_Position.y = j["Position"][1];
	}

	if (j.contains("Target") && !j["Target"].is_null()) {
		Identification identification;
		Identification::from_json(j["Target"], identification);

		auto entity =
			CreateRef<Entity>(*Registry::GetInstance(),
							  static_cast<entt::entity>(identification.id));
		followCamera.SetTarget(entity);
	} else {
		followCamera.SetTarget(nullptr);
	}
}
} // namespace Blazr
