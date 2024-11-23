#include "Blazr/Core/Log.h"
#include "Blazr/Renderer/Renderer2D.h"
#include "Blazr/Systems/AnimationSystem.h"
#include "Blazr/Systems/ScriptingSystem.h"
#include "Editor.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace Blazr {
	static float zoomLevel = 1.0f;
	// audio
	static float volumeLevel = 0.0f;
	// varijable za backend:
	// transform
	static float positionX = 0.0f; 
	static float positionY = 0.0f;
	static float scaleX = 1.0f;
	static float scaleY = 1.0f;
	static float rotation = 0.0f;
	// identification
	static char name[128] = "";
	static char groupName[128] = "";
	// sprite
	static float spriteWidth = 0.0f;
	static float spriteHeight = 0.0f;
	static float layer = 0.0f;
	static float sheetX = 0.0f;
	static float sheetY = 0.0f;
	// physics
	static float density = 0.0f;
	static float friction = 0.0f;
	static float restitution = 0.0f;
	static float gravityScale = 0.0f;
	static bool isSensor = false;
	static bool isFixedRotation = false;
	// za GUI
	static bool showCodeEditor = false;
	static bool showTransformComponent = false;
	static bool showIdentificationComponent = false;
	static bool showSpriteComponent = false;
	static bool showPhysicsComponent = false;
	static bool showColorTab = false;

	static std::string luaScriptContent = ""; // Sadržaj Lua skripte
	static char	luaScriptBuffer[1024 * 16]; // Buffer za unos teksta (podešen na 16KB)

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

	void Editor::renderTransformComponent(ImVec2 &cursorPos) {
		ImGui::SetCursorPos(cursorPos);
		ImGui::Separator();
		ImGui::Text("Transform");
		ImGui::SameLine();
		cursorPos.y += 28;

		// Pozicija (Position)
		ImGui::SetCursorPos(cursorPos);
		ImGui::Text("Position");
		cursorPos.y += 18;
		ImGui::SetCursorPos(cursorPos);
		ImGui::PushItemWidth(105);
		ImGui::Text("X");
		ImGui::SameLine();
		ImGui::InputFloat("##PositionX", &positionX, 0.1f, 1.0f, "%.1f");
		ImGui::SameLine();
		ImGui::Text("Y");
		ImGui::SameLine();
		ImGui::InputFloat("##PositionY", &positionY, 0.1f, 1.0f, "%.1f");
		ImGui::PopItemWidth();
		cursorPos.y += 25;

		// Skaliranje (Scale)
		ImGui::SetCursorPos(cursorPos);
		ImGui::Text("Scale");
		cursorPos.y += 18;

		ImGui::SetCursorPos(cursorPos);
		ImGui::PushItemWidth(105);
		ImGui::Text("X");
		ImGui::SameLine();
		ImGui::InputFloat("##ScaleX", &scaleX, 0.1f, 1.0f, "%.1f");
		ImGui::SameLine();
		ImGui::Text("Y");
		ImGui::SameLine();
		ImGui::InputFloat("##ScaleY", &scaleY, 0.1f, 1.0f, "%.1f");
		ImGui::PopItemWidth();
		cursorPos.y += 30;

		// Rotacija (Rotation)
		ImGui::SetCursorPos(cursorPos);
		ImGui::Text("Rotation");
		cursorPos.x += 15;
		cursorPos.y += 20;
		ImGui::SetCursorPos(cursorPos);
		ImGui::InputFloat("##Rotation", &rotation, 0.1f, 1.0f, "%.1f");
		cursorPos.x -= 15;
		cursorPos.y += 35;
	}

	void Editor::renderIdentificationComponent(ImVec2 &cursorPos) {
		ImGui::SetCursorPos(cursorPos);
		ImGui::Separator();
		ImGui::Text("Identification");
		cursorPos.y += 30;

		ImGui::SetCursorPos(cursorPos);
		ImGui::Text("Name");
		cursorPos.x += 43;
		ImGui::SetCursorPos(cursorPos);
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::InputText("###nameObject", name, IM_ARRAYSIZE(name));
		cursorPos.x -= 43;
		cursorPos.y += 25;
		ImGui::SetCursorPos(cursorPos);
		ImGui::Text("Group");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::InputText("###group", groupName, IM_ARRAYSIZE(groupName));
		cursorPos.y += 35;
	}

	void Editor::renderSpriteComponent(ImVec2& cursorPos) {
		ImGui::SetCursorPos(cursorPos);
		ImGui::Separator();
		ImGui::Text("Sprite");
		cursorPos.y += 30;
		ImGui::SetCursorPos(cursorPos);
		ImGui::Text("Textures");
		cursorPos.y -= 3;
		cursorPos.x += 65;
		ImGui::SetCursorPos(cursorPos);
		const char *textures[] = {"Texture1", "Texture2"};
		static int selectedTextureIndex =	-1;
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::BeginCombo("##TexturesDropdown", selectedTextureIndex == -1 ? "Choose a component" : textures[selectedTextureIndex])) {
			for (int i = 0; i < IM_ARRAYSIZE(textures); i++) {
				bool isSelected = (selectedTextureIndex == i);
				if (ImGui::Selectable(textures[i], isSelected)) {
					selectedTextureIndex = i;
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		//spriteWidth
		cursorPos.x -= 64;
		cursorPos.y += 28;
		ImGui::SetCursorPos(cursorPos);
		ImGui::Text("Width");
		cursorPos.x += 65;
		cursorPos.y -= 3;
		ImGui::SetCursorPos(cursorPos);
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::InputFloat("##spriteWidth", &spriteWidth, 0.1f, 1.0f,
							"%.1f");
		// spriteHeight
		cursorPos.x -= 65;
		cursorPos.y += 28;
		ImGui::SetCursorPos(cursorPos);
		ImGui::Text("Height");
		cursorPos.x += 65;
		cursorPos.y -= 3;
		ImGui::SetCursorPos(cursorPos);
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::InputFloat("##spriteHeight", &spriteHeight, 0.1f, 1.0f,
							"%.1f");
		// layer
		cursorPos.x -= 65;
		cursorPos.y += 28;
		ImGui::SetCursorPos(cursorPos);
		ImGui::Text("Layer");
		cursorPos.x += 65;
		cursorPos.y -= 3;
		ImGui::SetCursorPos(cursorPos);
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::InputFloat("##layer", &layer, 0.1f, 1.0f, "%.1f");
		// sprite sheet position
		cursorPos.x -= 65;
		cursorPos.y += 30;
		ImGui::SetCursorPos(cursorPos);
		ImGui::Text("Sprite Sheet Position");
		cursorPos.y += 20;
		ImGui::SetCursorPos(cursorPos);
		ImGui::PushItemWidth(95);
		ImGui::Text("X");
		ImGui::SameLine();
		ImGui::InputFloat("##sheetX", &sheetX, 0.1f, 1.0f, "%.1f");
		ImGui::SameLine();
		ImGui::Text(" Y");
		ImGui::SameLine();
		ImGui::InputFloat("##sheetY", &sheetY, 0.1f, 1.0f, "%.1f");
		// cursorPos.x -= 15;
		cursorPos.y += 35;
	}
	void Editor::renderPhysicsComponent(ImVec2 &cursorPos) {
		ImGui::SetCursorPos(cursorPos);
		ImGui::Separator();
		ImGui::Text("Physics");
		// type
		cursorPos.y += 30;
		ImGui::SetCursorPos(cursorPos);
		ImGui::Text("Type");
		cursorPos.y -= 3;
		cursorPos.x += 90;
		ImGui::SetCursorPos(cursorPos);
		const char *types[] = {"Static", "Dynamic", "Kinematic"};
		static int selectedTypeIndex = -1; 
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::BeginCombo("##TypesDropdown", selectedTypeIndex == -1 ? "Choose a type" : types[selectedTypeIndex])) {
			for (int i = 0; i < IM_ARRAYSIZE(types); i++) {
				bool isSelected = (selectedTypeIndex == i);
				if (ImGui::Selectable(types[i], isSelected)) {
					selectedTypeIndex = i;
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		// density
		cursorPos.x -= 90;
		cursorPos.y += 28;
		ImGui::SetCursorPos(cursorPos);
		ImGui::Text("Density");
		cursorPos.x += 90;
		cursorPos.y -= 3;
		ImGui::SetCursorPos(cursorPos);
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::InputFloat("##density", &density, 0.1f, 1.0f, "%.1f");
		// friction
		cursorPos.x -= 90;
		cursorPos.y += 28;
		ImGui::SetCursorPos(cursorPos);
		ImGui::Text("Friction");
		cursorPos.x += 90;
		cursorPos.y -= 3;
		ImGui::SetCursorPos(cursorPos);
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::InputFloat("##friction", &friction, 0.1f, 1.0f, "%.1f");
		// restitution
		cursorPos.x -= 90;
		cursorPos.y += 28;
		ImGui::SetCursorPos(cursorPos);
		ImGui::Text("Restitution");
		cursorPos.x += 90;
		cursorPos.y -= 3;
		ImGui::SetCursorPos(cursorPos);
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::InputFloat("##restitution", &restitution, 0.1f, 1.0f, "%.1f");
		// gravityScale
		cursorPos.x -= 90;
		cursorPos.y += 28;
		ImGui::SetCursorPos(cursorPos);
		ImGui::Text("GravityScale");
		cursorPos.x += 90;
		cursorPos.y -= 3;
		ImGui::SetCursorPos(cursorPos);
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::InputFloat("##gravityScale", &gravityScale, 0.1f, 1.0f, "%.1f");
		// isSensor
		cursorPos.x -= 90;
		cursorPos.y += 28;
		ImGui::SetCursorPos(cursorPos);
		ImGui::Text("isSensor");
		ImGui::SameLine(ImGui::GetContentRegionAvail().x - 12);
		ImGui::Checkbox("##isSensorCheckbox", &isSensor);
		// isFixedRotation
		cursorPos.y += 25;
		ImGui::SetCursorPos(cursorPos);
		ImGui::Text("isFixedRotation");
		ImGui::SameLine(ImGui::GetContentRegionAvail().x - 12);
		ImGui::Checkbox("##isFixedRotation", &isFixedRotation);
		
		cursorPos.y += 35;
	}

	void Editor::RenderImGui() { 
		
		ImGuiViewport *viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |=	ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

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
		ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNavFocus);
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
			ImVec2 cursorPos = ImVec2(10, 55);
			if (ImGui::BeginTabBar("DetailsTabs")) {
				if (ImGui::BeginTabItem("Components")) {
					if (showTransformComponent) {
						renderTransformComponent(cursorPos);
					}
					if (showIdentificationComponent) {
						renderIdentificationComponent(cursorPos);
					}
					if (showSpriteComponent) {
						renderSpriteComponent(cursorPos);
					}
					//if (showPhysicsComponent) {
						renderPhysicsComponent(cursorPos);
					//}

					if (ImGui::BeginPopupContextWindow("AddComponentPopup", ImGuiPopupFlags_MouseButtonRight)) {
						if (ImGui::MenuItem("Add Component")) {
							showComponentWindow = true;
						}
						ImGui::EndPopup();
					}
					if (showComponentWindow) {
						ImGui::SetNextWindowPos(ImVec2(300, 300), ImGuiCond_FirstUseEver);
						ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);

						ImGui::Begin("Choose Component", &showComponentWindow, ImGuiWindowFlags_NoCollapse);
						ImGui::Text("CHOOSE COMPONENT TO ADD");
						ImGui::Dummy(ImVec2(0.0f, 10.0f));
						const char *components[] = {"Sprite Component", "Box Collider", "Animation", "Identification", "RigidBody", "Transform Component", "Physics"};
						static int selectedComponentIndex =	-1; // -1 znači da nijedna komponenta nije izabrana
						ImGui::SetNextItemWidth(170.0f);
						if (ImGui::BeginCombo("##ComponentDropdown", selectedComponentIndex == -1 ? "Choose a component" : components[selectedComponentIndex])) {
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
						ImGui::Dummy(ImVec2(0.0f, ImGui::GetContentRegionAvail().y - 30.0f));
						// Dugmad "Add" i "Cancel"
						if (ImGui::Button("Add", ImVec2(80, 0))) {
							if (selectedComponentIndex != -1) {
								// Logika za dodavanje izabrane komponente
								switch (selectedComponentIndex) {
								case 0:
									// metoda addSprite
									showSpriteComponent = true;
									showColorTab = true;
									break;
								case 1:
									// metoda addBoxCollider
									break;
								case 2:
									// metoda Animation
									break;
								case 3:
									showIdentificationComponent = true;
									break;
								case 4:
									// RigidBody
									break;
								case 5:
									showTransformComponent = true; 
									break;
								case 6:
									showPhysicsComponent = true;
									break;
								}
								showComponentWindow = false; // Zatvori prozor nakon dodavanja
							}
						}
						ImGui::SameLine();
						if (ImGui::Button("Cancel", ImVec2(80, 0))) {
							showComponentWindow = false; 
						}
						ImGui::End();
					}
					ImGui::EndTabItem();
				}
				if (showColorTab) {
					if (ImGui::BeginTabItem("Color")) {
						ImGui::Text("Color settings");
						static ImVec4 sceneColor =
							ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
						if (ImGui::ColorPicker3("Game object\ncolor",
												(float *)&sceneColor)) {
							Ref<Registry> registry = m_Scene->GetRegistry();
							auto view =
								registry->GetRegistry().view<SpriteComponent>();
							for (auto entity : view) {
								auto &sprite =
									view.get<SpriteComponent>(entity);
								sprite.color = {sceneColor.x, sceneColor.y,
												sceneColor.z, sceneColor.w};
							}
						}
						ImGui::EndTabItem();
					}
				}
				ImGui::EndTabBar();
			}
		} 
		else {
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
					//
				}
				ImGui::SameLine();
				if (ImGui::Button("Stop")) {		
					//
				}
				ImGui::SameLine(ImGui::GetContentRegionAvail().x - 30);

				if (ImGui::Button("Code")) {
					showCodeEditor = !showCodeEditor; // Prikazuje ili skriva prozor za kod
					// Učitavanje Lua skripte samo prilikom prvog otvaranja editora
					if (showCodeEditor) {
						luaScriptContent = "Lua kod\n";
						strncpy(luaScriptBuffer, luaScriptContent.c_str(),
								sizeof(luaScriptBuffer));
					}
				}
				// Create a child window within the "Scene 1" tab for the Game View
				ImGui::BeginChild("GameViewChild", ImVec2(0, 0), true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

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

				ImGui::SetCursorPos(ImVec2(35, 53));
				ImGui::Text("Volume");

				ImGui::SetCursorPos(ImVec2(39, 68));
				ImGui::Text("%.3f", volumeLevel);

				ImGui::SetCursorPos(ImVec2(22, 90));
				ImGui::VSliderFloat("###volume", ImVec2(68, 180), &volumeLevel,
									0.0f, 1.0f, "");
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
