#include "Blazr/Core/Log.h"
#include "Blazr/Renderer/Renderer2D.h"
#include "Editor.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"

namespace Blazr {

Editor::Editor() { Init(); }

Editor::~Editor() { Shutdown(); }

void Editor::Init() {
	BLZR_CORE_INFO("Initializing Editor...");

	Renderer2D::Init();
	m_Renderer = Renderer2D();

	InitImGui();
}

void Editor::InitImGui() {
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(m_Window->GetWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 450");
}

void Editor::Run() {
	while (!glfwWindowShouldClose(m_Window->GetWindow())) {
		// Poll events
		m_Window->onUpdate();

		// Start ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		RenderImGui();

		// Render ImGui on top of the scene
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}

void Editor::RenderImGui() {
	ImGui::Begin("Editor Controls");

	// Menu bar example
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Open...")) {
				// Open file logic here
			}
			if (ImGui::MenuItem("Save As...")) {
				// Save file logic here
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Undo")) {
				// Undo action
			}
			if (ImGui::MenuItem("Redo")) {
				// Redo action
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	static float zoomLevel = 1.0f;
	if (ImGui::SliderFloat("Camera Zoom", &zoomLevel, 0.1f, 5.0f)) {
		m_Window->GetCamera()->SetScale(zoomLevel);
	}

	ImGui::Text("Rendering API - Blazr");

	ImGui::End();
}

void Editor::Shutdown() {
	Renderer2D::Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

} // namespace Blazr
