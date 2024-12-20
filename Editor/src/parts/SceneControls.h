#include "../Editor.h"
#include "Blazr/Core/Core.h"
#include "Blazr/Core/Log.h"
#include "Blazr/Project/Project.h"
#include "Blazr/Project/ProjectSerializer.h"
#include "Blazr/Renderer/CameraController.h"
#include "Blazr/Scene/TilemapScene.h"
#include "imgui.h"

namespace Blazr {
static bool showSceneAddDialog = false;
static char inputText[128];
void RenderSceneControls(bool &showCodeEditor, std::string &luaScriptContent,
						 char *luaScriptBuffer, Ref<Project> project) {

	if (ImGui::Button("+")) {
		showSceneAddDialog = true;
	}
	if (showSceneAddDialog) {

		auto scenes = project->GetScenes();
		ImVec2 windowSize(400, 150);
		ImVec2 windowPos(100, 100);
		ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(windowPos, ImGuiCond_FirstUseEver);
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse;
		if (ImGui::Begin("Choose Dialog", nullptr, windowFlags)) {
			ImGui::Text("Enter scene name:");
			ImGui::InputText("##input", inputText, IM_ARRAYSIZE(inputText));
			if (ImGui::Button("TileMapScene")) {
				Canvas canvas{16, 16};
				Ref<TilemapScene> tilemapScene =
					CreateRef<TilemapScene>(canvas);
				Project::GetActive()->AddScene(tilemapScene->GetName(),
											   tilemapScene);
				showSceneAddDialog = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("AddScene")) {
				Ref<Scene> newScene = CreateRef<Scene>();
				newScene->SetName(inputText);
				Project::GetActive()->AddScene(inputText, newScene);
				showSceneAddDialog = false;
				strcpy(inputText, "");
			}
		}
		ProjectSerializer::Serialize(
			Project::GetActive(), Project::GetActive()->GetProjectDirectory() /
									  Project::GetActive()->GetConfig().name);
		ImGui::End();
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
