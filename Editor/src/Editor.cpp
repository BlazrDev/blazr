#include "Blazr/Core/Log.h"
#include "Blazr/Renderer/Renderer2D.h"
#include "Editor.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include "Blazr/Ecs/Registry.h"
#include "Blazr/Ecs/Entity.h"

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

	glfwInit();
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable
	// Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// ImGui::StyleColorsClassic();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform
	// windows can look identical to regular ones.
	ImGuiStyle &style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(m_Window->GetWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 410");
}

void Editor::Begin() {

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	//ImGuizmo::BeginFrame();
}

void Editor::End() {

	ImGuiIO &io = ImGui::GetIO();
	io.DisplaySize =
		ImVec2(1280.f, 720.f);

	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		GLFWwindow *backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

static float zoomLevel = 1.0f;
Ref<Camera2D> camera = Camera2D::GetInstance();

void Editor::Run() {

	std::unique_ptr<Registry> registry = std::make_unique<Registry>();
	glm::vec2 pos = {0.f, 0.f};
	glm::vec2 size = {200.f, 200.f};
	glm::vec4 color = {1.f, 1.f, 1.f, 1.f};

	Entity entity = Entity(*registry, "Ent1", "G1");
	auto &transform =
		entity.AddComponent<TransformComponent>(TransformComponent{
			.position = pos, .scale = glm::vec2(1.0f, 1.0f), .rotation = 0.0f});

	auto &sprite = entity.AddComponent<SpriteComponent>(
		SpriteComponent{.width = size[0],
						.height = size[1],
						.startX = 10,
						.startY = 30,
						.texturePath = "assets/chammy.png"});

	while (!glfwWindowShouldClose(m_Window->GetWindow())) {
		// Poll events
		glfwPollEvents();
		//m_Window->onUpdate();
		camera->Update();

		//BLZR_CLIENT_INFO("AAAAAAa: {0}", camera->GetScale());
		Renderer2D::BeginScene(*camera);
		auto view =
			registry->GetRegistry().view<TransformComponent, SpriteComponent>();
		for (auto entity : view) {
			auto &transform = view.get<TransformComponent>(entity);
			transform.scale = glm::vec2(zoomLevel, zoomLevel);
			transform.scale = glm::vec2(3.f, 1.f);
			Renderer2D::DrawQuad(*registry, entity);
		}

		Renderer2D::Flush();
		
		//zoomLevel = camera->GetScale();
		

		// Start ImGui frame
		/*ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();*/

		Begin();
		RenderImGui();
		End();
		glfwSwapBuffers(m_Window->GetWindow());
		//Renderer2D::Clear();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//m_Renderer.Clear();
		// Render ImGui on top of the scene
		//ImGui::Render();
		//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}

void Editor::RenderImGui() {
	//ImGui::Begin("Editor Controls");

	// Menu bar example
	//if (ImGui::BeginMainMenuBar()) {
	//	if (ImGui::BeginMenu("File")) {
	//		if (ImGui::MenuItem("Open...")) {
	//			// Open file logic here
	//		}
	//		if (ImGui::MenuItem("Save As...")) {
	//			// Save file logic here
	//		}
	//		ImGui::EndMenu();
	//	}
	//	if (ImGui::BeginMenu("Edit")) {
	//		if (ImGui::MenuItem("Undo")) {
	//			// Undo action
	//		}
	//		if (ImGui::MenuItem("Redo")) {
	//			// Redo action
	//		}
	//		ImGui::EndMenu();
	//	}
	//	ImGui::EndMainMenuBar();
	//}

	
	if (ImGui::SliderFloat("Camera Zoom", &zoomLevel, 0.1f, 5.0f)) {
		camera->SetScale(zoomLevel);
		//BLZR_CLIENT_INFO("AAAAAAa: {0}", camera->GetScale());
	}

	//ImGui::Text("Rendering API - Blazr");

	//ImGui::End();
}

void Editor::Shutdown() {
	Renderer2D::Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

} // namespace Blazr
