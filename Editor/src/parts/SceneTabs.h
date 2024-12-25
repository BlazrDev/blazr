#include "../Editor.h"
#include "Blazr/Core/Log.h"
#include "Blazr/Project/Project.h"
#include "Blazr/Project/ProjectSerializer.h"
#include "Blazr/Renderer/CameraController.h"
#include "imgui.h"

namespace Blazr {

void RenderSceneTabs(Editor &editor) {
	if (ImGui::BeginTabBar("SceneTabs")) {

		if (Project::GetActive() == nullptr) {
			ImGui::EndTabBar();
			return;
		}
		for (const auto &[name, scene] : Project::GetActive()->GetScenes()) {
			if (ImGui::BeginTabItem(name.c_str())) {
				editor.SetActiveScene(scene);
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}
}

void RenderSceneToTexture(Ref<Scene> activeScene) {
	activeScene->m_GameFrameBuffer->Bind();
	// activeScene->m_GameFrameBuffer->Cleanup();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (activeScene != nullptr) {

		if (!CameraController::paused) {
			activeScene->Update();
		}
		activeScene->Render();

	} else {
		BLZR_CORE_WARN("Active scene is null");
	}

	activeScene->m_GameFrameBuffer->Unbind();
}

void RenderActiveScene(Ref<Scene> activeScene, ImGuiWindowFlags flags) {
	if (activeScene) {
		ImGui::BeginChild("GameViewChild", ImVec2(0, 0), true, flags);

		CameraController::gameViewWindow = ImGui::IsWindowHovered();
		ImVec2 windowSize = {1280, 720}; // ImGui::GetContentRegionAvail();
		int newWidth = static_cast<int>(windowSize.x);
		int newHeight = static_cast<int>(windowSize.y);

		if (newWidth != activeScene->m_GameFrameBuffer->GetWidth() ||
			newHeight != activeScene->m_GameFrameBuffer->GetHeight()) {
			activeScene->m_GameFrameBuffer->Resize(newWidth, newHeight);
		}

		RenderSceneToTexture(activeScene);
		ImGui::Image((intptr_t)activeScene->m_GameFrameBuffer->GetTextureID(),
					 windowSize, ImVec2(0, 1), ImVec2(1, 0));
		ImGui::EndChild();
	}
}
} // namespace Blazr
