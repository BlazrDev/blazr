#include "../Editor.h"
#include "Blazr/Core/Core.h"
#include "Blazr/Project/Project.h"
#include "Blazr/Project/ProjectSerializer.h"
#include "Blazr/Renderer/CameraController.h"
#include "Blazr/Scene/TilemapScene.h"
#include "imgui.h"

namespace Blazr {
void RenderSceneControls(bool &showCodeEditor, std::string &luaScriptContent,
						 char *luaScriptBuffer) {
	if (ImGui::Button("+")) {
		std::string newSceneName =
			"Untitled " +
			std::to_string(Project::GetActive()->GetScenes().size() + 1);
		if (true) {
			Ref<TilemapScene> tilemapScene = CreateRef<TilemapScene>();
			Project::GetActive()->AddScene(tilemapScene->GetName(),
										   tilemapScene);
		} else {
			Ref<Scene> newScene = CreateRef<Scene>();
			Project::GetActive()->AddScene(newSceneName, newScene);
		}

		ProjectSerializer::Serialize(
			Project::GetActive(), Project::GetActive()->GetProjectDirectory() /
									  Project::GetActive()->GetConfig().name);
	}
	ImGui::SameLine();
	if (ImGui::Button("Play")) {
		CameraController::paused = false;
	}
	ImGui::SameLine();
	if (ImGui::Button("Stop")) {
		CameraController::paused = true;
	}

	ImGui::SameLine();
	if (ImGui::Button("Code")) {
		showCodeEditor = !showCodeEditor;

		if (showCodeEditor) {
			luaScriptContent = "Lua code\n";
			strncpy(luaScriptBuffer, luaScriptContent.c_str(),
					sizeof(luaScriptBuffer));
		}
	}
}
} // namespace Blazr
