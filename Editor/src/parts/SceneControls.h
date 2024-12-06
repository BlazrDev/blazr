#include "../Editor.h"
#include "Blazr/Project/Project.h"
#include "Blazr/Project/ProjectSerializer.h"
#include "Blazr/Renderer/CameraController.h"
#include "imgui.h"

namespace Blazr {
void RenderSceneControls(bool &showCodeEditor, std::string &luaScriptContent,
						 char *luaScriptBuffer) {
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
