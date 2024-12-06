#include "../Editor.h"
#include "Blazr/Project/Project.h"
#include "Blazr/Project/ProjectSerializer.h"
#include "imgui.h"

namespace Blazr {
void RenderSceneTabs(Editor &editor) {
	if (ImGui::BeginTabBar("SceneTabs")) {
		for (const auto &[name, scene] : Project::GetActive()->GetScenes()) {
			if (ImGui::BeginTabItem(name.c_str())) {
				editor.SetActiveScene(scene);
				ImGui::EndTabItem();
			}
		}

		if (ImGui::Button("+")) {
			std::string newSceneName =
				"Untitled " +
				std::to_string(Project::GetActive()->GetScenes().size() + 1);
			Ref<Scene> newScene = CreateRef<Scene>();
			Project::GetActive()->AddScene(newSceneName, newScene);

			ProjectSerializer::Serialize(
				Project::GetActive(),
				Project::GetActive()->GetProjectDirectory() /
					Project::GetActive()->GetConfig().name);
		}

		ImGui::EndTabBar();
	}
}

void RenderSceneToTexture(Ref<Scene> activeScene,
						  Ref<FrameBuffer> gameFrameBuffer) {
	gameFrameBuffer->Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (activeScene != nullptr) {

		if (!CameraController::paused) {
			activeScene->Update();
		}
		activeScene->Render();
	} else {
		BLZR_CORE_WARN("Active scene is null");
	}

	gameFrameBuffer->Unbind();
}

void RenderActiveScene(Ref<Scene> activeScene,
					   Ref<FrameBuffer> gameFrameBuffer) {
	if (activeScene) {
		ImGui::BeginChild("GameViewChild", ImVec2(0, 0), true,
						  ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

		ImVec2 windowSize = ImGui::GetContentRegionAvail();
		int newWidth = static_cast<int>(windowSize.x);
		int newHeight = static_cast<int>(windowSize.y);

		if (newWidth != gameFrameBuffer->GetWidth() ||
			newHeight != gameFrameBuffer->GetHeight()) {
			gameFrameBuffer->Resize(newWidth, newHeight);
		}

		RenderSceneToTexture(activeScene, gameFrameBuffer);
		ImGui::Image((intptr_t)gameFrameBuffer->GetTextureID(), windowSize,
					 ImVec2(0, 1), ImVec2(1, 0));
		ImGui::EndChild();
	}
}
} // namespace Blazr
