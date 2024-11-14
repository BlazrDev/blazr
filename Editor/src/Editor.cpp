#include "Blazr/Core/Log.h"
#include "Blazr/Renderer/Renderer2D.h"
#include "Blazr/Systems/AnimationSystem.h"
#include "Blazr/Systems/ScriptingSystem.h"
#include "Editor.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace Blazr 
{
	static float zoomLevel = 1.0f;
	static float volumeLevel = 0.0f;
	static float pitchLevel = 0.0f;
	static float positionX = 0.0f;
	static float positionY = 0.0f;
	static float scaleX = 1.0f;
	static float scaleY = 1.0f;
	static float rotation = 0.0f;
	static char name[128] = "";
	static char groupName[128] = "";
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
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigWindowsMoveFromTitleBarOnly = true;

		ImGui::StyleColorsDark();

		ImGui_ImplGlfw_InitForOpenGL(m_Window->GetWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void Editor::Run() {
		while (!glfwWindowShouldClose(m_Window->GetWindow())) {

			glfwPollEvents();
			m_Window->onUpdate();
			// zoomLevel = m_Window->GetCamera()->GetScale();

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

		glfwSwapBuffers(m_Window->GetWindow());
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
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |=
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	// Begin a full-screen window that acts as the main dock space
	glfwSwapInterval(1);
	ImGui::Begin("Main DockSpace", nullptr, window_flags);
	ImGui::PopStyleVar(2);
	// Docking space
	ImGui::DockSpace(ImGui::GetID("MainDockSpace"), ImVec2(0.0f, 0.0f));


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



	//--------------------------------------------------------------1. box - Editor Controls--------------------------------
	ImGui::SetNextWindowSize(ImVec2(230, 90), ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
	ImGui::Begin("Editor Controls", nullptr);
	ImGui::SetCursorPos(ImVec2(50, 20));
	ImGui::Text("Camera Zoom");
	// Zooming
	if (ImGui::SliderFloat("##ZoomSlider", &zoomLevel, 0.1f, 5.0f)) {
		m_Scene->GetCamera().SetScale(zoomLevel);
	}


	//--------------------------------------------------------------2. box - Scene---------------------------------
	ImGui::SetNextWindowSize(ImVec2(230, 170), ImGuiCond_Once);
	ImVec2 editorControlsPos = ImGui::GetWindowPos();
	ImVec2 editorControlsSize = ImGui::GetWindowSize();
	ImGui::SetNextWindowPos(ImVec2(editorControlsPos.x, editorControlsPos.y + editorControlsSize.y));
	ImGui::Begin("Scene", nullptr);
	for (int objIdx = 0; objIdx < numberOfComponents; ++objIdx) {
		std::string gameObjectName = "GameObject" + std::to_string(objIdx + 1);
		if (ImGui::Selectable(gameObjectName.c_str())) {
			selectedComponent = "ObjectDetails-" + gameObjectName;
			showDetails = true;
		}
	}
	ImVec2 scenePos = ImGui::GetWindowPos();
	ImVec2 sceneSize = ImGui::GetWindowSize();
	ImGui::End();


	//---------------------------------------------------------------3. box - Object details---------------------------------
	ImGui::SetNextWindowSize(ImVec2(310, 700), ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 310, 20),
							ImGuiCond_Once);
	//ImGui::Begin(selectedComponent.c_str(), nullptr);
	// Tab bar for different sections of details
	if (ImGui::Begin(selectedComponent.c_str(), nullptr)) 
	{
		if (ImGui::BeginTabBar("DetailsTabs")) {
			if (ImGui::BeginTabItem("Transform")) {
				ImGui::Separator();
				ImGui::Text("Transform");
				ImGui::SameLine();
				ImVec2 p = ImGui::GetCursorScreenPos(); // position of next
														// element after Text
				float lineWidth =
					ImGui::GetContentRegionAvail().x; // remaining line length
				float lineHeight = 2.0f;			  // line thickness
				//
				float textHeight = ImGui::GetTextLineHeight();
				float lineY = p.y + 2.0 + textHeight / 2.0f - lineHeight / 2.0f;
				// drawing line
				ImGui::GetWindowDrawList()->AddRectFilled(
					ImVec2(p.x, lineY),
					ImVec2(p.x + lineWidth, lineY + lineHeight),
					IM_COL32(255, 255, 255, 255));

				{
					// Position input
					ImGui::PushItemWidth(130);
					ImGui::SetCursorPos(ImVec2(10, 80));
					ImGui::Text("Position");
					ImGui::Text("X");
					ImGui::SameLine();
					ImGui::InputFloat("##PositionX", &positionX, 0.1f, 1.0f,
									  "%.1f"); // Input za X koordinatu
					ImGui::SameLine();
					ImGui::Text("Y");
					ImGui::SameLine();
					ImGui::InputFloat("##PositionY", &positionY, 0.1f, 1.0f,
									  "%.1f"); // Input za Y koordinatu

					// Scale input
					ImGui::SetCursorPos(ImVec2(10, 130));
					ImGui::Text("Scale");
					ImGui::Text("X");
					ImGui::SameLine();
					ImGui::InputFloat("##ScaleX", &scaleX, 0.1f, 1.0f,
									  "%.1f"); // Input za skaliranje po X osi
					ImGui::SameLine();
					ImGui::Text("Y");
					ImGui::SameLine();
					ImGui::InputFloat("##ScaleY", &scaleY, 0.1f, 1.0f,
									  "%.1f"); // Input za skaliranje po Y osi

					// Rotation input
					ImGui::SetCursorPos(ImVec2(10, 180));
					ImGui::Text("Rotation");
					ImGui::PushItemWidth(130);
					ImGui::SetCursorPos(ImVec2(20, 200));
					ImGui::InputFloat("##Rotation", &rotation, 0.1f, 1.0f,
									  "%.1f"); // Input za rotaciju
					ImGui::PopItemWidth();
				}

				ImGui::SetCursorPos(ImVec2(10, 240));
				ImGui::Separator();
				ImGui::Text("Identification");
				ImGui::SameLine();
				ImVec2 p2 = ImGui::GetCursorScreenPos(); // position of next
														 // element after Text
				float lineWidth2 =
					ImGui::GetContentRegionAvail().x; // remaining line length
				float lineHeight2 = 2.0f;			  // line thickness
				//
				float textHeight2 = ImGui::GetTextLineHeight();
				float lineY2 =
					p2.y + 2.0 + textHeight2 / 2.0f - lineHeight2 / 2.0f;
				// drawing line
				ImGui::GetWindowDrawList()->AddRectFilled(
					ImVec2(p2.x, lineY2),
					ImVec2(p2.x + lineWidth2, lineY2 + lineHeight2),
					IM_COL32(255, 255, 255, 255));

				{
					ImGui::SetCursorPos(ImVec2(10, 270));
					ImGui::Text("Name");
					ImGui::SetCursorPos(ImVec2(50, 270));
					ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
					ImGui::InputText("###nameObject", name, IM_ARRAYSIZE(name));
					ImGui::PopItemWidth();
					ImGui::SetCursorPos(ImVec2(10, 293));
					ImGui::Text("Group");
					ImGui::SetCursorPos(ImVec2(50, 293));
					ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
					ImGui::InputText("###group", groupName,
									 IM_ARRAYSIZE(groupName));
					ImGui::PopItemWidth();
				}
				ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x,
									500.0f)); // visina prostora

				// Detekcija desnog klika na prazan prostor
				if (ImGui::IsItemHovered() &&
					ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
					ImGui::OpenPopup("AddComponentPopup");
				}

				// Popup za dodavanje komponenti
				if (ImGui::BeginPopup("AddComponentPopup")) {
					if (ImGui::Selectable("Add Component 1")) {
						// Logika za dodavanje komponente 1
					}
					ImGui::EndPopup();
				}
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Color")) {
				ImGui::Text("Color settings");
				static ImVec4 sceneColor = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
				if (ImGui::ColorPicker3("Game object\ncolor",
										(float *)&sceneColor)) {
					Ref<Registry> registry = m_Scene->GetRegistry();
					auto view = registry->GetRegistry().view<SpriteComponent>();
					for (auto entity : view) {
						auto &sprite = view.get<SpriteComponent>(entity);
						sprite.color = {sceneColor.x, sceneColor.y,
										sceneColor.z, sceneColor.w};
					}
				}
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::End();
	}

	//-----------------------------------------------------------4. box - Camera box with tabs---------------------------------
	ImGui::SetNextWindowSize(ImVec2(740, 398), ImGuiCond_Once);
	 ImGui::SetNextWindowPos(ImVec2(editorControlsPos.x + editorControlsSize.x, editorControlsPos.y));
	ImGui::Begin("Camera", nullptr);
	if (ImGui::BeginTabBar("DetailsTabs")) {

		if (ImGui::BeginTabItem("Scene 1")) {

			// Create a child window within the "Scene 1" tab for the Game
			// View
			ImGui::BeginChild("GameViewChild", ImVec2(0, 0), true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

			// Get the available space in the child window to render the
			// Game View ImGui::Begin("Game View");
			ImVec2 windowSize = ImGui::GetContentRegionAvail();
			int newWidth = static_cast<int>(windowSize.x);
			int newHeight = static_cast<int>(windowSize.y);

			if (newWidth != m_GameFrameBuffer->GetWidth() || newHeight != m_GameFrameBuffer->GetHeight()) {
				m_GameFrameBuffer->Resize(newWidth, newHeight);
			}
			// Render the scene to texture
			RenderSceneToTexture();
			// Display the framebuffer texture in the Game View
			ImGui::Image((intptr_t)m_GameFrameBuffer->GetTextureID(), windowSize, ImVec2(0, 1), ImVec2(1, 0));
			ImGui::EndChild(); // End the Game View child window
			ImGui::EndTabItem(); // End "Scene 1" tab item
		}
		if (ImGui::BeginTabItem("+")) {
			//NAPRAVITI
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImVec2 cameraPos = ImGui::GetWindowPos();
	ImVec2 cameraSize = ImGui::GetWindowSize();
	ImGui::End();

	//-------------------------------------------------------------5. box - audio/templates---------------------------------
	ImGui::SetNextWindowSize(ImVec2(740, 300), ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(editorControlsPos.x + editorControlsSize.x, cameraPos.y + cameraSize.y));
	ImGui::Begin("BOX", nullptr);
	if (ImGui::BeginTabBar("Tabs")) {

		if (ImGui::BeginTabItem("Audio")) {

			ImGui::SetCursorPos(ImVec2(41, 60));
			ImGui::Text("Volume");
			ImGui::SetCursorPos(ImVec2(165, 60));
			ImGui::Text("Pitch");

			ImGui::SetCursorPos(ImVec2(44, 73));
			ImGui::Text("%.3f", volumeLevel);
			ImGui::SetCursorPos(ImVec2(165, 73));
			ImGui::Text("%.3f", pitchLevel);

			ImGui::SetCursorPos(ImVec2(20, 90));
			ImGui::VSliderFloat("###volume", ImVec2(80, 180), &volumeLevel, 0.0f, 1.0f, "");
			ImGui::SetCursorPos(ImVec2(140, 90));
			ImGui::VSliderFloat("###pitch", ImVec2(80, 180), &pitchLevel, 0.0f, 1.0f, "");
			//Backend: varijable volumeLevel i pitchLevel

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Templates")) {
			ImGui::Text("  Volume");

			// Poèetna i maksimalna visina za kretanje kruga
			ImVec2 start_pos = ImGui::GetCursorScreenPos();
			float slider_height = 150.0f; // Visina "slidera"
			float slider_width = 60.0f;	  // Širina "slidera"

			// Kreiramo prostor za nevidljivi "slider"
			ImGui::InvisibleButton("##slider",ImVec2(slider_width, slider_height));

			// Poèetna pozicija kruga
			static float progress = 0.5f; // Vrednost napretka izmeðu 0.0 i 1.0
			float circle_y = start_pos.y + progress * slider_height;

			// Proverimo da li je kliknut i pomeran miš unutar "slidera"
			if (ImGui::IsItemActive()) {
				// Relativna pozicija miša unutar slidera
				float mouse_y = ImGui::GetIO().MousePos.y;
				// Raèunanje pozicije kruga u odnosu na poziciju kursora
				progress = (mouse_y - start_pos.y) / slider_height;
				progress = progress < 0.0f ? 0.0f : (progress > 1.0f ? 1.0f : progress);

				circle_y = start_pos.y + progress * slider_height;
			}

			// Crtanje pozadinske linije za slider
			ImDrawList *draw_list = ImGui::GetWindowDrawList();
			ImVec2 line_start =	ImVec2(start_pos.x + slider_width / 2, start_pos.y);
			ImVec2 line_end = ImVec2(start_pos.x + slider_width / 2, start_pos.y + slider_height);
			draw_list->AddLine(line_start, line_end, IM_COL32(200, 200, 200, 255), 4.0f);

			// Crtanje kruga koji predstavlja napredak
			ImVec2 circle_pos = ImVec2(line_start.x, circle_y);
			draw_list->AddCircleFilled(circle_pos, 10.0f, IM_COL32(43, 98, 128, 255));

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
	ImGui::End();

	ImGui::End();
	ImGui::End();
	

	// Game View window (docked within the main window)
	 //ImGui::Begin("Game View", nullptr, ImGuiConfigFlags_DockingEnable);
	
	 //// Adjust the Game View framebuffer size if the window is resized
	 //ImVec2 windowSize = ImGui::GetContentRegionAvail();
	 //int newWidth = static_cast<int>(windowSize.x);
	 //int newHeight = static_cast<int>(windowSize.y);
	
	 //if (newWidth != m_GameFrameBuffer->GetWidth() ||
	 //	newHeight != m_GameFrameBuffer->GetHeight()) {
	 //	m_GameFrameBuffer->Resize(newWidth, newHeight);
	 //}
	
	 //// Render the scene to texture
	 //RenderSceneToTexture();
	
	 //// Display the framebuffer texture in the Game View
	 //ImGui::Image((intptr_t)m_GameFrameBuffer->GetTextureID(), windowSize,
	 //			 ImVec2(0, 1), ImVec2(1, 0));
	 //ImGui::End();

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
