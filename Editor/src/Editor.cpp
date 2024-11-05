#include "Blazr/Core/Log.h"
#include "Blazr/Renderer/Camera2D.h"
#include "Blazr/Renderer/RenderContext.h"
#include "Blazr/Renderer/Renderer2D.h"
#include "Editor.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace Blazr {

Editor::Editor() : m_Window(nullptr) { Init(); }

Editor::~Editor() {
	if (m_Window) {
		delete m_Window;
	}
}

void Editor::Init() {
	BLZR_CORE_INFO("Initializing Editor...");

	WindowProperties props("Blazr Editor", 1280, 720);
	m_Window = Window::create(props);
	m_Camera = Camera2D(1280, 720);

	if (!m_Window) {
		BLZR_CORE_ERROR("Failed to create window!");
		return;
	}

	m_Window->setEventCallback(
		[this](Event &e) { BLZR_CORE_TRACE("Event received in Editor"); });

	m_Window->setVSync(true);

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
		m_Window->onUpdate();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		RenderImGui();
		m_Renderer.BeginScene(m_Camera);

		m_Renderer.Flush();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(m_Window->GetWindow());
	}
}

void Editor::RenderImGui() {
	// Begin ImGui frame for editor controls
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

	static float zoomLevel = 1.0f; // Starting zoom level
	if (ImGui::SliderFloat("Camera Zoom", &zoomLevel, 0.1f, 5.0f)) {
		m_Camera.SetScale(zoomLevel); // Apply the zoom level to the camera
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
