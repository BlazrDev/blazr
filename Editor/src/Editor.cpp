#include "Blazr/Core/Log.h"
#include "Blazr/Renderer/Renderer2D.h"
#include "Blazr/Systems/AnimationSystem.h"
#include "Blazr/Systems/ScriptingSystem.h"
#include "Editor.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace Blazr {

Editor::Editor() { Init(); }

Editor::~Editor() { Shutdown(); }

void Editor::Init() {
	BLZR_CORE_INFO("Initializing Editor...");

	Renderer2D::Init();
	m_Scene = CreateRef<Scene>();
	auto animationSystem =
		std::make_shared<AnimationSystem>(*(m_Scene->GetRegistry()).get());
	auto scriptingSystem =
		std::make_shared<ScriptingSystem>(*(m_Scene->GetRegistry()).get());

	m_Scene->GetRegistry()->AddToContext(animationSystem);
	m_Scene->GetRegistry()->AddToContext(scriptingSystem);
	m_GameFrameBuffer = CreateRef<FrameBuffer>(1280, 720);
	m_Renderer = Renderer2D();

	InitImGui();
}

void Editor::InitImGui() {
	if (!glfwInit()) {
		BLZR_CORE_ERROR("GLFW initialization failed!");
		return;
	}
	if (!m_Window->GetWindow()) {
		glfwTerminate();
		BLZR_CORE_ERROR("GLFW window creation failed!");
		return;
	}

	glfwMakeContextCurrent(m_Window->GetWindow());
	glfwSwapInterval(1);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(m_Window->GetWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 450");
}

static float zoomLevel = 1.0f;
void Editor::Run() {
	while (!glfwWindowShouldClose(m_Window->GetWindow())) {

		m_Window->onUpdate();
		zoomLevel = m_Window->GetCamera()->GetScale();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		RenderImGui();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		ImGuiIO &io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			GLFWwindow *backupContext = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();

			glfwMakeContextCurrent(backupContext);
		}
	}
}

void Editor::RenderImGui() {

	ImGui::Begin("Editor Controls");

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Open...")) {
			}
			if (ImGui::MenuItem("Save As...")) {
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Undo")) {
			}
			if (ImGui::MenuItem("Redo")) {
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (ImGui::SliderFloat("Camera Zoom", &zoomLevel, 0.1f, 5.0f)) {
		m_Scene->GetCamera().SetScale(zoomLevel);
	}

	ImGui::Text("Rendering API - Blazr");
	ImGui::End();

	ImGui::Begin("Game View");

	ImVec2 windowSize = ImGui::GetContentRegionAvail();
	int newWidth = static_cast<int>(windowSize.x);
	int newHeight = static_cast<int>(windowSize.y);

	if (newWidth != m_GameFrameBuffer->GetWidth() ||
		newHeight != m_GameFrameBuffer->GetHeight()) {
		m_GameFrameBuffer->Resize(newWidth, newHeight);
	}

	RenderSceneToTexture();

	ImGui::Image((intptr_t)m_GameFrameBuffer->GetTextureID(), windowSize,
				 ImVec2(0, 1), ImVec2(1, 0));
	ImGui::End();
}

void Editor::Shutdown() {
	Renderer2D::Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Editor::RenderSceneToTexture() {
	m_GameFrameBuffer->Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_Scene->Update();
	m_Scene->Render();

	m_GameFrameBuffer->Unbind();
}

} // namespace Blazr
