#include "Blazr/Core/Log.h"
#include "Blazr/Renderer/Renderer2D.h"
#include "Blazr/Systems/AnimationSystem.h"
#include "Blazr/Systems/ScriptingSystem.h"
#include "Editor.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace Blazr {
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
static bool showCodeEditor = false;
static std::string luaScriptContent = ""; // Sadržaj Lua skripte
static char
	luaScriptBuffer[1024 * 16]; // Buffer za unos teksta (podešen na 16KB)

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
	//io.ConfigWindowsMoveFromTitleBarOnly = true;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(m_Window->GetWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 410");
	//glDisable(GL_SCISSOR_TEST);
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

		//glClear(GL_COLOR_BUFFER_BIT);

		ImGuiIO &io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			GLFWwindow *backupContext = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();

			glfwMakeContextCurrent(backupContext);
		}

		
	//glfwSwapBuffers(m_Window->GetWindow());
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

	static bool showGameObjectDetails = false;
	static std::string selectedGameObject = "ObjectDetails";
	int numberOfComponents = 3;

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

	//// Zooming
	//if (ImGui::SliderFloat("##ZoomSlider", &zoomLevel, 0.1f, 5.0f)) {
	//	m_Scene->GetCamera().SetScale(zoomLevel);
	//}----------------------------------------------------------1. box - Scene---------------------------------

	int widthSize = m_Window->getWidth();
	int heightSize = m_Window->getHeight() - 20;

	ImGui::SetNextWindowSize(ImVec2(270, heightSize));
	ImGui::SetNextWindowPos(ImVec2(0,19));
	ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
					 ImGuiWindowFlags_NoNavFocus);
	for (int objIdx = 0; objIdx < numberOfComponents; ++objIdx) {
		std::string gameObjectName = "GameObject" + std::to_string(objIdx + 1);
		if (ImGui::Selectable(gameObjectName.c_str())) {
			selectedGameObject = "ObjectDetails-" + gameObjectName;
			showGameObjectDetails = true;
		}
	}
	ImVec2 scenePos = ImGui::GetWindowPos();
	ImVec2 sceneSize = ImGui::GetWindowSize();
	ImGui::End();

	//---------------------------------------------------------------2. box - Object details---------------------------------
	ImGui::SetNextWindowSize(ImVec2(270, heightSize));
	ImGui::SetNextWindowPos(ImVec2(widthSize - 270, 19));
	ImGui::Begin(selectedGameObject.c_str(), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNavFocus);
	static bool showComponentWindow = false;
	if (showGameObjectDetails) {
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
				float textHeight = ImGui::GetTextLineHeight();
				float lineY = p.y + 2.0 + textHeight / 2.0f - lineHeight / 2.0f;
				// drawing line
				ImGui::GetWindowDrawList()->AddRectFilled(
					ImVec2(p.x, lineY),
					ImVec2(p.x + lineWidth, lineY + lineHeight),
					IM_COL32(255, 255, 255, 255));

				{
					// Position input
					ImGui::PushItemWidth(100);
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
					if (ImGui::MenuItem("Add Component")) {
						ImGui::CloseCurrentPopup(); // Zatvaranje popup-a
						showComponentWindow = true;
					}
					ImGui::EndPopup();
				}
				if (showComponentWindow) {
					ImGui::SetNextWindowPos(ImVec2(300, 300),
											ImGuiCond_FirstUseEver);
					ImGui::SetNextWindowSize(ImVec2(300, 200),
											 ImGuiCond_FirstUseEver);

					ImGui::Begin("Choose Component", &showComponentWindow,
								 ImGuiWindowFlags_NoCollapse);
					ImGui::Text("CHOOSE COMPONENT TO ADD");
					ImGui::Dummy(ImVec2(0.0f, 10.0f));
					const char *components[] = {"Sprite Component",
												"Box Collider", "Animation"};
					static int selectedComponentIndex =
						-1; // -1 znači da nijedna komponenta nije izabrana
					ImGui::SetNextItemWidth(170.0f);
					if (ImGui::BeginCombo(
							"##ComponentDropdown",
							selectedComponentIndex == -1
								? "Choose a component"
								: components[selectedComponentIndex])) {
						for (int i = 0; i < IM_ARRAYSIZE(components); i++) {
							bool isSelected = (selectedComponentIndex == i);
							if (ImGui::Selectable(components[i], isSelected)) {
								selectedComponentIndex = i;
							}
							if (isSelected) {
								ImGui::SetItemDefaultFocus();
							}
						}
						ImGui::EndCombo();
					}

					ImGui::Dummy(
						ImVec2(0.0f, ImGui::GetContentRegionAvail().y - 30.0f));

					// Dugmad "Add" i "Cancel"
					if (ImGui::Button("Add", ImVec2(80, 0))) {
						if (selectedComponentIndex != -1) {
							// Logika za dodavanje izabrane komponente
							switch (selectedComponentIndex) {
							case 0:
								// TO DO: dodati sprite details u DETAILS BOX
								break;
							case 1:
								// TO DO: dodati box collider details U DETAILS
								// BOX
								break;
							case 2:
								// TO DO: dodati animation details u DETAILS BOX
								break;
							}
							showComponentWindow =
								false; // Zatvori prozor nakon dodavanja
						}
					}

					ImGui::SameLine();
					if (ImGui::Button("Cancel", ImVec2(80, 0))) {
						showComponentWindow =
							false; // Zatvori prozor bez dodavanja
					}
					ImGui::End();
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
	} else {
		ImGui::Separator();
		ImGui::Dummy(ImVec2());
		ImGui::SetWindowFontScale(1.2f);
		ImGui::Text("Choose GameObject for details!");
		ImGui::SetWindowFontScale(1.0f);
	}
	ImGui::End();

	//-----------------------------------------------------------3. box - Camera box with tabs---------------------------------
	ImGui::SetNextWindowSize(ImVec2(widthSize - 230 - 310, heightSize - 300));
	ImGui::SetNextWindowPos(ImVec2(270, 19));
	ImGui::Begin("Camera", nullptr,
				 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	if (ImGui::BeginTabBar("##DetailsTabs")) {

		if (ImGui::BeginTabItem("Scene 1")) {
			if (ImGui::Button("Play")) {
				// Logika za pokretanje scene
			}
			ImGui::SameLine();
			if (ImGui::Button("Stop")) {
				// Logika za zaustavljanje scene
			}
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 30);

			if (ImGui::Button("Code")) {
				showCodeEditor =
					!showCodeEditor; // Prikazuje ili skriva prozor za kod
				// Učitavanje Lua skripte samo prilikom prvog otvaranja editora
				if (showCodeEditor) {
					luaScriptContent = "Lua kod\n";
					strncpy(luaScriptBuffer, luaScriptContent.c_str(),
							sizeof(luaScriptBuffer));
				}
			}
			// Create a child window within the "Scene 1" tab for the Game View
			ImGui::BeginChild("GameViewChild", ImVec2(0, 0), true,
							  ImGuiWindowFlags_NoMove |
								  ImGuiWindowFlags_NoResize);

			// Get the available space in the child window to render the Game
			// View ImGui::Begin("Game View");
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
			ImGui::EndChild();	 // End the Game View child window
			ImGui::EndTabItem(); // End "Scene 1" tab item
		}
		if (ImGui::BeginTabItem("+")) {
			// TO DO
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	if (showCodeEditor) {
		ImGui::Begin("Lua Script Editor", &showCodeEditor,
					 ImGuiWindowFlags_AlwaysAutoResize);

		// Prikaz editora za unos Lua koda
		ImGui::InputTextMultiline(
			"##luaScript", luaScriptBuffer, sizeof(luaScriptBuffer),
			ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16),
			ImGuiInputTextFlags_AllowTabInput);

		if (ImGui::Button("Save")) {
			// Logika za čuvanje Lua koda u fajl
			luaScriptContent = luaScriptBuffer;
		}
		ImGui::SameLine();
		if (ImGui::Button("Close")) {
			showCodeEditor = false; // Zatvara editor
		}

		ImGui::End();
	}

	ImVec2 cameraPos = ImGui::GetWindowPos();
	ImVec2 cameraSize = ImGui::GetWindowSize();
	ImGui::End();

	//-------------------------------------------------------------4. box - audio/templates---------------------------------
	ImGui::SetNextWindowSize(ImVec2(widthSize - 230 - 310, 300));
	ImGui::SetNextWindowPos(ImVec2(270, heightSize - 281));
	ImGui::Begin("BOX", nullptr,
				 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
	const char *items[] = {"TEXTURES", "FONTS", "MUSIC", "SOUNDFX", "SCENES"};
	static int current_item = 0;

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
			ImGui::VSliderFloat("###volume", ImVec2(80, 180), &volumeLevel,
								0.0f, 1.0f, "");
			ImGui::SetCursorPos(ImVec2(140, 90));
			ImGui::VSliderFloat("###pitch", ImVec2(80, 180), &pitchLevel, 0.0f,
								1.0f, "");
			// Backend: varijable volumeLevel i pitchLevel

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Assets")) {

			if (ImGui::BeginCombo("Asset Type", items[current_item])) {
				for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
					if (ImGui::Selectable(items[n], current_item == n)) {
						current_item = n;
					}
					if (current_item == n) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			switch (current_item) {
			case 0: // TEXTURES
				ImGui::Text("Texture");
				break;
			case 1: // FONTS
				ImGui::Text("Font");
				break;
			case 2: // MUSIC
				ImGui::Text("Music");
				break;
			case 3: // SOUNDFX
				ImGui::Text("Sound");
				break;
			case 4: // SCENES
				ImGui::Text("Scene");
				break;
			default:
				ImGui::Text("Select an asset type to view its content.");
				break;
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Logs")) {
			// TO DO
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
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
