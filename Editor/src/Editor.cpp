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
	// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigWindowsMoveFromTitleBarOnly = true;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(m_Window->GetWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 410");
}

static float zoomLevel = 1.0f;
void Editor::Run() {
	while (!glfwWindowShouldClose(m_Window->GetWindow())) {

		m_Window->onUpdate();
		zoomLevel = m_Scene->GetCamera().GetScale();

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

	ImGuiViewport *viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGuiWindowFlags window_flags =
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
		ImGuiConfigFlags_ViewportsEnable | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove;
	window_flags |=
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	// Begin a full-screen window that acts as the main dock space
	ImGui::Begin("Main DockSpace", nullptr, window_flags);
	ImGui::PopStyleVar(2);

	// Docking space
	ImGui::DockSpace(ImGui::GetID("MainDockSpace"), ImVec2(0.0f, 0.0f),
					 ImGuiDockNodeFlags_PassthruCentralNode);
	static bool showDetails = false;
	static std::string selectedComponent = "ObjectDetails";
	int numberOfComponents = 10;

	// Menu bar
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
		if (ImGui::BeginMenu("Tools")) {
			// Tools action
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Settings")) {
			// Settings action
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help")) {
			// Help action
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	//---------------------------------1. box - Editor
	// Controls---------------------------------
	ImGui::SetNextWindowSize(ImVec2(270, 120), ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
	// ImGui::SetNextWindowCollapsed(true, ImGuiCond_Once);
	ImGui::Begin("Editor Controls", nullptr);
	// ImGuiID dockspace_id = ImGui::GetID("Editor Controls");
	// ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));

	// Zooming
	if (ImGui::SliderFloat("Camera Zoom", &zoomLevel, 0.1f, 5.0f)) {
		m_Scene->GetCamera().SetScale(zoomLevel);
	}
	//---------------------------------2. box -
	// Scene---------------------------------
	ImGui::SetNextWindowCollapsed(true, ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(270, 120), ImGuiCond_Once);

	ImVec2 editorControlsPos = ImGui::GetWindowPos();
	ImVec2 editorControlsSize = ImGui::GetWindowSize();
	ImGui::SetNextWindowPos(ImVec2(editorControlsPos.x,
								   editorControlsPos.y + editorControlsSize.y));
	ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoMove);

	for (int objIdx = 0; objIdx < numberOfComponents; ++objIdx) {
		std::string gameObjectName = "GameObject" + std::to_string(objIdx + 1);
		if (ImGui::TreeNode(gameObjectName.c_str())) {
			ImGui::BeginChild(("ChildWindow" + std::to_string(objIdx)).c_str(),
							  ImVec2(0, 150), true,
							  ImGuiWindowFlags_HorizontalScrollbar);

			// Components for each GameObject
			for (int compIdx = 0; compIdx < numberOfComponents; ++compIdx) {
				std::string componentName =
					"Component " + std::to_string(compIdx + 1);
				if (ImGui::Selectable(componentName.c_str())) {
					selectedComponent =
						"ObjectDetails-" + gameObjectName + "-" + componentName;
					showDetails = true;
				}
			}

			ImGui::EndChild();
			ImGui::TreePop();
		}
	}

	ImVec2 scenePos = ImGui::GetWindowPos();
	ImVec2 sceneSize = ImGui::GetWindowSize();
	ImGui::End();

	//---------------------------------3. box - Object
	// details---------------------------------
	ImGui::SetNextWindowSize(ImVec2(310, 700), ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 310, 20),
							ImGuiCond_Once);

	ImGui::Begin(selectedComponent.c_str(), nullptr, ImGuiWindowFlags_NoMove);
	// Tab bar for different sections of details
	if (ImGui::BeginTabBar("DetailsTabs")) {

		if (ImGui::BeginTabItem("Transform")) {
			ImGui::Text("Transform settings here");
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Color")) {
			ImGui::Text("Color settings");
			static ImVec4 sceneColor = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
			if (ImGui::ColorPicker3("Scene Color", (float *)&sceneColor)) {
				// Boja je promenjena, možeš da dodate neki kod za
				// ažuriranje npr. ažuriranje boje objekta u sceni.
				// HACK: SRKELSS SE IGRA
				Ref<Registry> registry = m_Scene->GetRegistry();
				auto view = registry->GetRegistry().view<SpriteComponent>();
				for (auto entity : view) {
					auto &sprite = view.get<SpriteComponent>(entity);
					sprite.color = {sceneColor.x, sceneColor.y, sceneColor.z,
									sceneColor.w};
				}
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::End();

	//---------------------------------4. box - Camera box with
	// tabs---------------------------------
	ImGui::SetNextWindowSize(ImVec2(698, 398), ImGuiCond_Once);
	// ImGui::SetNextWindowCollapsed(true, ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(editorControlsPos.x + editorControlsSize.x,
								   editorControlsPos.y));
	ImGui::Begin("Camera", nullptr, ImGuiWindowFlags_NoMove);
	if (ImGui::BeginTabBar("DetailsTabs")) {

		if (ImGui::BeginTabItem("Scene 1")) {
			ImGui::Text("Scene 1");

			// Create a child window within the "Scene 1" tab for the Game
			// View
			ImGui::BeginChild("GameViewChild", ImVec2(0, 0), true,
							  ImGuiWindowFlags_NoMove |
								  ImGuiWindowFlags_NoResize);

			// Get the available space in the child window to render the
			// Game View ImGui::Begin("Game View");

			ImVec2 windowSize = ImGui::GetContentRegionAvail();
			int newWidth = static_cast<int>(windowSize.x);
			int newHeight = static_cast<int>(windowSize.y);

			if (newWidth != m_GameFrameBuffer->GetWidth() ||
				newHeight != m_GameFrameBuffer->GetHeight()) {
				m_GameFrameBuffer->Resize(newWidth, newHeight);
			}

			// Render the scene to texture
			RenderSceneToTexture();

			// Display the framebuffer texture in the Game View
			ImGui::Image((intptr_t)m_GameFrameBuffer->GetTextureID(),
						 windowSize, ImVec2(0, 1), ImVec2(1, 0));
			// ImGui::End();
			ImGui::EndChild(); // End the Game View child window

			ImGui::EndTabItem(); // End "Scene 1" tab item
		}
		if (ImGui::BeginTabItem("+")) {
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImVec2 cameraPos = ImGui::GetWindowPos();
	ImVec2 cameraSize = ImGui::GetWindowSize();
	ImGui::End();
	//---------------------------------5. box -
	// audio/templates---------------------------------
	ImGui::SetNextWindowSize(ImVec2(698, 300), ImGuiCond_Once);
	// ImGui::SetNextWindowCollapsed(true, ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(editorControlsPos.x + editorControlsSize.x,
								   cameraPos.y + cameraSize.y));
	ImGui::Begin("BOX", nullptr);

	if (ImGui::BeginTabBar("Tabs")) {

		if (ImGui::BeginTabItem("Audio")) {
			ImGui::Text("Audio transformation");
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Templates")) {
			ImGui::Text("Choose template");
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
	ImGui::End();
	ImGui::End();

	// Game View window (docked within the main window)
	// ImGui::Begin("Game View");
	//
	// // Adjust the Game View framebuffer size if the window is resized
	// ImVec2 windowSize = ImGui::GetContentRegionAvail();
	// int newWidth = static_cast<int>(windowSize.x);
	// int newHeight = static_cast<int>(windowSize.y);
	//
	// if (newWidth != m_GameFrameBuffer->GetWidth() ||
	// 	newHeight != m_GameFrameBuffer->GetHeight()) {
	// 	m_GameFrameBuffer->Resize(newWidth, newHeight);
	// }
	//
	// // Render the scene to texture
	// RenderSceneToTexture();
	//
	// // Display the framebuffer texture in the Game View
	// ImGui::Image((intptr_t)m_GameFrameBuffer->GetTextureID(), windowSize,
	// 			 ImVec2(0, 1), ImVec2(1, 0));
	// ImGui::End();
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
