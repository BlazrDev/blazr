#include "Blazr/Core/Log.h"
#include "Blazr/Ecs/Components/AnimationComponent.h"
#include "Blazr/Ecs/Components/BoxColliderComponent.h"
#include "Blazr/Ecs/Components/Identification.h"
#include "Blazr/Ecs/Components/PhysicsComponent.h"
#include "Blazr/Ecs/Components/ScriptComponent.h"
#include "Blazr/Ecs/Components/SpriteComponent.h"
#include "Blazr/Ecs/Components/TileComponent.h"
#include "Blazr/Ecs/Components/TransformComponent.h"
#include "Blazr/Events/ApplicationEvent.h"
#include "Blazr/Events/Event.h"
#include "Blazr/Physics/Box2DWrapper.h"
#include "Blazr/Project/ProjectSerializer.h"
#include "Blazr/Renderer/CameraController.h"
#include "Blazr/Renderer/Renderer2D.h"
#include "Blazr/Resources/AssetManager.h"
#include "Blazr/Systems/AnimationSystem.h"
#include "Blazr/Systems/PhysicsSystem.h"
#include "Blazr/Systems/ScriptingSystem.h"
#include "Blazr/Systems/Sounds/SoundPlayer.h"
#include "Editor.h"
#include "box2d/b2_body.h"
#include "box2d/box2d.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "parts/SceneControls.h"
#include "parts/SceneTabs.h"
// #include "utils/FileDialog.h"
#include <GLFW/glfw3.h>
#include <memory>

namespace Blazr {
// layers
static int selectedLayerIndex = 0;
static bool showAddLayerPopup = false;
static char newLayerName[256] = "New Layer";
static int newLayerZIndex = 0;

static float pos = 0;
static bool mapflag = true;
static float zoomLevel = 1.0f;
// audio
static float volumeLevel = 0.0f;
// transform
static int posX = 0;
static int posY = 0;
static int newPosX = 0;
static int newPosY = 0;

static float scaleX = 1.0f;
static float scaleY = 1.0f;

static float newScaleX = 1.0f;
static float newScaleY = 1.0f;

// identification
static char name[128] = "";
static char groupName[128] = "";
// sprite
static char layer[128] = "";
static float spriteWidth = 0.0f;
static float spriteHeight = 0.0f;
static float sheetX = 0.0f;
static float sheetY = 0.0f;

// BoxCollider
static int widthBoxCollider = 16, heightBoxCollider = 16;
static int newWidthBoxCollider = 16, newHeightBoxCollider = 16;
static float offsetX = 0, offsetY = 0;
static float newOffsetX = 0, newOffsetY = 0;

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

// managers

static auto assetManager = AssetManager::GetInstance();
static auto soundPlayer = SoundPlayer::GetInstance();

static std::string luaScriptContent = ""; // Sadržaj Lua skripte
static char
	luaScriptBuffer[1024 * 16]; // Buffer za unos teksta (podešen na 16KB)

Editor::Editor() { Init(); }

Editor::~Editor() { Shutdown(); }

void Editor::Init() {
	BLZR_CORE_INFO("Initializing Editor...");

	Renderer2D::Init();
	// m_Scene = CreateRef<Scene>();
	auto animationSystem = std::make_shared<AnimationSystem>(*m_Registry.get());
	auto scriptingSystem = std::make_shared<ScriptingSystem>(*m_Registry.get());
	m_Registry->AddToContext(animationSystem);
	m_Registry->AddToContext(scriptingSystem);
	m_GameFrameBuffer = CreateRef<FrameBuffer>(1280, 720);
	m_Renderer = Renderer2D();
	InitImGui();
	Ref<Project> newProject = Project::New("StartProject");
	Project::SetActive(newProject);
	Ref<Scene> newScene = CreateRef<Scene>();
	newProject->AddScene("Scene1", newScene);
	m_ActiveScene = newScene;
	ProjectSerializer::Serialize(newProject, Project::GetProjectDirectory() /
												 newProject->GetConfig().name);
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

	glfwSetWindowUserPointer(m_Window->GetWindow(), &m_EventCallback);
	glfwSetFramebufferSizeCallback(
		m_Window->GetWindow(), [](GLFWwindow *window, int width, int height) {
			glViewport(0, 0, width, height);
		});

	glfwSetScrollCallback(
		m_Window->GetWindow(),
		[](GLFWwindow *window, double xOffset, double yOffset) {
			auto eventCallback = static_cast<std::function<void(Event &)> *>(
				glfwGetWindowUserPointer(window));

			if (eventCallback) {
				MouseScrolledEvent event(static_cast<float>(xOffset),
										 static_cast<float>(yOffset));
				(*eventCallback)(event);
			}
		});

	glfwSetWindowSizeCallback(
		m_Window->GetWindow(), [](GLFWwindow *window, int width, int height) {
			auto eventCallback = static_cast<std::function<void(Event &)> *>(
				glfwGetWindowUserPointer(window));
			if (eventCallback) {
				WindowResizeEvent event(width, height);
				(*eventCallback)(event);
			}
		});

	glfwSetWindowCloseCallback(m_Window->GetWindow(), [](GLFWwindow *window) {
		auto eventCallback = static_cast<std::function<void(Event &)> *>(
			glfwGetWindowUserPointer(window));

		if (eventCallback) {
			WindowCloseEvent event{};
			(*eventCallback)(event);
		}
	});
	m_EventCallback = [this](Event &e) { m_ActiveScene->onEvent(e); };

	glfwMakeContextCurrent(m_Window->GetWindow());
	glfwSwapInterval(1);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	// io.ConfigWindowsMoveFromTitleBarOnly = true;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(m_Window->GetWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 410");
	// glDisable(GL_SCISSOR_TEST);
}

void Editor::Run() {
	while (!glfwWindowShouldClose(m_Window->GetWindow())) {
		int width;
		int height;
		glfwGetWindowSize(m_Window->GetWindow(), &width, &height);
		m_Window->setWidth(width);
		m_Window->setHeight(height);
		glfwPollEvents();
		m_Window->onUpdate();

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
		// glfwSwapBuffers(m_Window->GetWindow());
	}

	glfwSwapBuffers(m_Window->GetWindow());
}

static std::string selectedGameObject = "ObjectDetails";
void Editor::RenderImGui() {
	ImVec2 cursorPos = ImVec2(10, 55);
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
	int numberOfComponents = 3;

	// Menu bar
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("New Project")) {
				/*std::string savePath = Blazr::FileDialog::SaveFile(
					"Blazr Project (*.blzr)\0*.blzr\0");
				if (!savePath.empty()) {
					Ref<Project> newProject = Project::New("UntitledProject");
					Ref<Scene> newScene = CreateRef<Scene>();
					newProject->AddScene("Scene1", newScene);
					newProject->GetConfig().StartSceneName = "Scene1";
					m_ActiveScene = newScene;

					newProject->SetProjectDirectory(
						std::filesystem::path(savePath).parent_path());
					newProject->GetConfig().name =
						std::filesystem::path(savePath).stem().string();

					ProjectSerializer::Serialize(newProject, savePath);
				}*/
			}

			if (ImGui::MenuItem("Open...")) {
				/*std::string openPath = Blazr::FileDialog::OpenFile(
					"Blazr Project (*.blzr)\0*.blzr\0");
				if (!openPath.empty()) {
					Ref<Project> loadedProject = Project::Load(openPath);
					if (loadedProject) {
						Project::SetActive(loadedProject);
						m_ActiveScene = loadedProject->GetScene(
							loadedProject->GetConfig().StartSceneName);
					} else {
						BLZR_CORE_ERROR("Failed to load project from: {}",
										openPath);
					}
				}*/
			}

			if (ImGui::MenuItem("Save As...")) {
				/*std::string savePath = Blazr::FileDialog::SaveFile(
					"Blazr Project (*.blzr)\0*.blzr\0");
				if (!savePath.empty()) {
					Ref<Project> active = Project::GetActive();
					if (active) {
						active->SetProjectDirectory(
							std::filesystem::path(savePath).parent_path());
						active->GetConfig().name =
							std::filesystem::path(savePath).stem().string();

						ProjectSerializer::Serialize(active, savePath);
					} else {
						BLZR_CORE_ERROR("No active project to save.");
					}
				}*/
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
	// if (ImGui::SliderFloat("##ZoomSlider", &zoomLevel, 0.1f, 5.0f)) {
	//	m_Scene->GetCamera().SetScale(zoomLevel);
	// }----------------------------------------------------------1. box -
	// Scene---------------------------------

	int widthSize = m_Window->getWidth();
	int heightSize = m_Window->getHeight() - 20;

	ImGui::SetNextWindowSize(ImVec2(270, heightSize));
	ImGui::SetNextWindowPos(ImVec2(0, 19));
	ImGui::Begin("Scene", nullptr,
				 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
					 ImGuiWindowFlags_NoNavFocus);
	auto view = m_Registry->GetRegistry().view<entt::entity>(
		entt::exclude<TileComponent>);

	for (auto entity : view) {
		auto &identification =
			m_Registry->GetRegistry().get<Identification>(entity);
		std::string gameObjectName = identification.name;
		if (ImGui::Selectable(gameObjectName.c_str())) {
			selectedGameObject = "ObjectDetails-" + gameObjectName;
			showGameObjectDetails = true;
		}
	}
	ImVec2 scenePos = ImGui::GetWindowPos();
	ImVec2 sceneSize = ImGui::GetWindowSize();
	ImGui::End();

	//---------------------------------------------------------------2. box -
	// Object details---------------------------------
	ImGui::SetNextWindowSize(ImVec2(270, heightSize));
	ImGui::SetNextWindowPos(ImVec2(widthSize - 270, 19));
	ImGui::Begin(selectedGameObject.c_str(), nullptr,
				 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
					 ImGuiWindowFlags_NoNavFocus);
	static bool showComponentWindow = false;
	if (showGameObjectDetails) {
		if (ImGui::BeginTabBar("DetailsTabs")) {
			if (ImGui::BeginTabItem("Components")) {
				if (!CameraController::paused) {
					ImGui::BeginDisabled(true);
				}
				for (auto entity : view) {
					std::string entityName =
						"ObjectDetails-" + m_Registry->GetRegistry()
											   .get<Identification>(entity)
											   .name;
					if (m_Registry->GetRegistry().all_of<TransformComponent>(
							entity) &&
						selectedGameObject == entityName) {
						auto &transform =
							m_Registry->GetRegistry().get<TransformComponent>(
								entity);

						renderTransformComponent(cursorPos, transform);
					}
					if (m_Registry->GetRegistry().all_of<SpriteComponent>(
							entity) &&
						selectedGameObject == entityName) {
						auto &sprite =
							m_Registry->GetRegistry().get<SpriteComponent>(
								entity);
						showColorTab = true;

						renderSpriteComponent(cursorPos, sprite);

						std::vector<Ref<Layer>> layers =
							m_ActiveScene->GetLayerManager()->GetAllLayers();
						auto l = m_ActiveScene->GetLayerByName(sprite.layer);
						if (l) {
							l->AddEntity(
								CreateRef<Entity>(*m_Registry, entity));
						}
					}
					if (m_Registry->GetRegistry().all_of<PhysicsComponent>(
							entity) &&
						selectedGameObject == entityName) {
						auto &physics =
							m_Registry->GetRegistry().get<PhysicsComponent>(
								entity);
						renderPhysicsComponent(cursorPos, physics);
					}

					if (m_Registry->GetRegistry().all_of<AnimationComponent>(
							entity) &&
						selectedGameObject == entityName) {
						auto &animation =
							m_Registry->GetRegistry().get<AnimationComponent>(
								entity);
						// renderTransformComponent(cursorPos);
					}
					if (m_Registry->GetRegistry().all_of<BoxColliderComponent>(
							entity) &&
						selectedGameObject == entityName) {
						auto &boxCollider =
							m_Registry->GetRegistry().get<BoxColliderComponent>(
								entity);
						// renderTransformComponent(cursorPos);
					}
					if (m_Registry->GetRegistry().all_of<Identification>(
							entity) &&
						selectedGameObject == entityName) {
						auto &identification =
							m_Registry->GetRegistry().get<Identification>(
								entity);
						renderIdentificationComponent(cursorPos,
													  identification);
					}

					if (m_Registry->GetRegistry().all_of<ScriptComponent>(
							entity) &&
						selectedGameObject == entityName) {
						auto &script =
							m_Registry->GetRegistry().get<ScriptComponent>(
								entity);
						renderScriptComponent(cursorPos, script);
					}
				}

				if (!CameraController::paused) {
					ImGui::EndDisabled();
				}
				if (ImGui::BeginPopupContextWindow(
						"AddComponentPopup",
						ImGuiPopupFlags_MouseButtonRight)) {
					if (ImGui::MenuItem("Add Component")) {
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
					const char *components[] = {
						"Sprite Component",	   "Box Collider", "Animation",
						"Transform Component", "Physics",	   "Script"};
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
								// metoda addSprite
								for (auto entity : view) {
									std::string entityName =
										"ObjectDetails-" +
										m_Registry->GetRegistry()
											.get<Identification>(entity)
											.name;

									if (!m_Registry->GetRegistry()
											 .all_of<SpriteComponent>(entity) &&
										selectedGameObject == entityName) {
										m_Registry->GetRegistry()
											.emplace<SpriteComponent>(entity);

										auto &sprite =
											m_Registry->GetRegistry()
												.get<SpriteComponent>(entity);
										sprite.generateObject(sprite.width,
															  sprite.height);
									}
								}

								break;
							case 1:
								// metoda addBoxCollider
								for (auto entity : view) {
									std::string entityName =
										"ObjectDetails-" +
										m_Registry->GetRegistry()
											.get<Identification>(entity)
											.name;

									if (!m_Registry->GetRegistry()
											 .all_of<BoxColliderComponent>(
												 entity) &&
										selectedGameObject == entityName) {
										m_Registry->GetRegistry()
											.emplace<BoxColliderComponent>(
												entity);
									}
								}

								break;
							case 2:
								// metoda Animation
								for (auto entity : view) {
									std::string entityName =
										"ObjectDetails-" +
										m_Registry->GetRegistry()
											.get<Identification>(entity)
											.name;

									if (!m_Registry->GetRegistry()
											 .all_of<AnimationComponent>(
												 entity) &&
										selectedGameObject == entityName) {
										m_Registry->GetRegistry()
											.emplace<AnimationComponent>(
												entity);
									}
								}

								break;
							case 3:
								for (auto entity : view) {
									std::string entityName =
										"ObjectDetails-" +
										m_Registry->GetRegistry()
											.get<Identification>(entity)
											.name;

									if (!m_Registry->GetRegistry()
											 .all_of<TransformComponent>(
												 entity) &&
										selectedGameObject == entityName) {
										m_Registry->GetRegistry()
											.emplace<TransformComponent>(
												entity);
									}
								}
								break;
							case 4:
								for (auto entity : view) {
									std::string entityName =
										"ObjectDetails-" +
										m_Registry->GetRegistry()
											.get<Identification>(entity)
											.name;

									if (!m_Registry->GetRegistry()
											 .all_of<PhysicsComponent>(
												 entity) &&
										selectedGameObject == entityName) {
										auto &physicsWorld =
											m_Registry->GetContext<
												std::shared_ptr<b2World>>();
										m_Registry->GetRegistry()
											.emplace<PhysicsComponent>(
												entity,
												std::forward<PhysicsComponent>(
													PhysicsComponent(
														physicsWorld,
														PhysicsAttributes{})));
									}
								}
								break;

							case 5:
								for (auto entity : view) {
									std::string entityName =
										"ObjectDetails-" +
										m_Registry->GetRegistry()
											.get<Identification>(entity)
											.name;

									if (!m_Registry->GetRegistry()
											 .all_of<ScriptComponent>(entity) &&
										selectedGameObject == entityName) {
										m_Registry->GetRegistry()
											.emplace<ScriptComponent>(
												entity, ScriptComponent{});
									}
								}
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
			if (showColorTab) {
				if (!CameraController::paused) {
					ImGui::BeginDisabled(true);
				}
				if (ImGui::BeginTabItem("Color")) {
					ImGui::Text("Color settings");
					static ImVec4 sceneColor = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
					if (ImGui::ColorPicker3("Game object\ncolor",
											(float *)&sceneColor)) {
						for (auto entity : view) {
							std::string entityName =
								"ObjectDetails-" +
								m_Registry->GetRegistry()
									.get<Identification>(entity)
									.name;
							if (m_Registry->GetRegistry()
									.all_of<SpriteComponent>(entity) &&
								selectedGameObject == entityName) {
								auto &sprite =
									m_Registry->GetRegistry()
										.get<SpriteComponent>(entity);
								sprite.color = {sceneColor.x, sceneColor.y,
												sceneColor.z, sceneColor.w};
							}
						}
					}
					ImGui::EndTabItem();
				}
				if (!CameraController::paused) {
					ImGui::EndDisabled();
				}
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

	//-----------------------------------------------------------3. box
	//-
	// Camera
	// box with tabs---------------------------------
	ImGui::SetNextWindowSize(ImVec2(widthSize - 230 - 310, heightSize - 300));
	ImGui::SetNextWindowPos(ImVec2(270, 19));
	ImGui::Begin("Camera", nullptr,
				 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

	RenderSceneTabs(*this);
	RenderSceneControls(showCodeEditor, luaScriptContent, luaScriptBuffer);
	RenderActiveScene(m_ActiveScene, m_GameFrameBuffer);

	ImVec2 cameraPos = ImGui::GetWindowPos();
	ImVec2 cameraSize = ImGui::GetWindowSize();
	ImGui::End();

	//-------------------------------------------------------------4.
	// box -
	// audio/templates---------------------------------
	ImGui::SetNextWindowSize(ImVec2(widthSize - 230 - 310, 300));
	ImGui::SetNextWindowPos(ImVec2(270, heightSize - 281));
	ImGui::Begin("BOX", nullptr,
				 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
					 ImGuiWindowFlags_NoTitleBar);
	const char *items[] = {"TEXTURES", "FONTS", "MUSIC", "SOUNDFX", "SCENES"};
	static int current_item = 0;

	if (ImGui::BeginTabBar("Tabs")) {

		if (ImGui::BeginTabItem("Audio")) {
			if (ImGui::BeginTabBar("AudioSettingsTabBar")) {

				if (ImGui::BeginTabItem("Music")) {

					ImGui::SetCursorPos(ImVec2(35, 63));
					ImGui::BeginChild("MusicContainer", ImVec2(0, 220), false,
									  ImGuiWindowFlags_HorizontalScrollbar);
					ImGui::SetCursorPosX(35);
					ImGui::SetCursorPosY(16);
					for (const auto &pair : assetManager->getAllMusic()) {
						ImGui::BeginGroup();
						float groupWidth = 68.0f;
						float textOffset =
							(groupWidth -
							 ImGui::CalcTextSize(pair.first.c_str()).x) /
							2.0f;
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() +
											 textOffset);
						ImGui::Text(pair.first.c_str());
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);
						std::string label = "##" + pair.first;
						if (ImGui::VSliderInt(label.c_str(), ImVec2(68, 160),
											  &soundPlayer->musicVolume, 0, 100,
											  "%d")) {
							soundPlayer->MusicVolume(soundPlayer->musicVolume);
						}
						ImGui::EndGroup();
						ImGui::SameLine();
						ImGui::SetCursorPosY(13);
					}
					ImGui::EndChild();
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Effects")) {

					ImGui::BeginChild("EfffectsContainer", ImVec2(0, 220),
									  false,
									  ImGuiWindowFlags_HorizontalScrollbar);
					ImGui::SetCursorPos(ImVec2(35, 16));
					for (const auto &pair : assetManager->getAllEffects()) {
						ImGui::BeginGroup();
						float groupWidth = 68.0f;
						float textOffset =
							(groupWidth -
							 ImGui::CalcTextSize(pair.first.c_str()).x) /
							2.0f;

						ImGui::SetCursorPosX(ImGui::GetCursorPosX() +
											 textOffset);
						ImGui::Text(pair.first.c_str());
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);
						std::string label = "##" + pair.first;
						if (ImGui::VSliderInt(label.c_str(), ImVec2(68, 160),
											  &soundPlayer->channelVolumes
												   [pair.second->getChannel()],
											  0, 100, "%d")) {
							soundPlayer->EffectVolume(
								pair.second->getChannel(),
								soundPlayer->channelVolumes
									[pair.second->getChannel()]);
						}

						ImGui::EndGroup();
						ImGui::SameLine();
						ImGui::SetCursorPosY(13);
					}

					ImGui::EndChild();
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}

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

			auto &logEntries = Blazr::Log::getImGuiSink()->getEntries();

			if (ImGui::BeginChild("LogScrolling")) {
				for (const auto &entry : logEntries) {
					ImVec4 color;
					switch (entry.level) {
					case spdlog::level::trace:
						color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
						break;
					case spdlog::level::debug:
						color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
						break;
					case spdlog::level::info:
						color = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
						break;
					case spdlog::level::warn:
						color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
						break;
					case spdlog::level::err:
					case spdlog::level::critical:
						color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
						break;
					default:
						color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
						break;
					}
					ImGui::TextColored(color, "%s", entry.message.c_str());
				}
			}
			ImGui::EndChild();
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

void Blazr::Editor::setEventCallback(const Window::EventCallbackFn &callback) {
	m_EventCallback = callback;
}

void Editor::renderScriptComponent(ImVec2 &cursorPos, ScriptComponent &script) {
	ImGui::SetCursorPos(cursorPos);
	ImGui::Separator();
	ImGui::Text("Script Component");

	// Script Path
	cursorPos.y += 30;
	ImGui::SetCursorPos(cursorPos);
	ImGui::Text("Script Path");
	cursorPos.x += 90;
	cursorPos.y -= 3;
	ImGui::SetCursorPos(cursorPos);

	static char scriptPathBuffer[256];
	strcpy(scriptPathBuffer, script.scriptPath.c_str());
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	if (ImGui::InputText("##ScriptPathInput", scriptPathBuffer,
						 sizeof(scriptPathBuffer))) {
		script.scriptPath = scriptPathBuffer;
	}

	cursorPos.x -= 90;
	cursorPos.y += 28;

	// Update Function Check
	ImGui::SetCursorPos(cursorPos);
	ImGui::Text("Has Update Function");
	ImGui::SameLine(ImGui::GetContentRegionAvail().x - 12);
	bool hasUpdate = script.update.valid();
	ImGui::Checkbox("##UpdateFunctionCheckbox", &hasUpdate);

	cursorPos.y += 25;

	// Render Function Check
	ImGui::SetCursorPos(cursorPos);
	ImGui::Text("Has Render Function");
	ImGui::SameLine(ImGui::GetContentRegionAvail().x - 12);
	bool hasRender = script.render.valid();
	ImGui::Checkbox("##RenderFunctionCheckbox", &hasRender);

	cursorPos.y += 35;

	// Reload Script Button
	ImGui::SetCursorPos(cursorPos);
	if (ImGui::Button("Reload Script")) {
		// Trigger a script reload process
		sol::state lua;
		try {
			auto result = lua.safe_script_file(script.scriptPath);
			if (result.valid()) {
				std::string scriptName = script.scriptPath.substr(
					script.scriptPath.find_last_of("/\\") + 1);
				scriptName = scriptName.substr(0, scriptName.find_last_of('.'));

				sol::table scriptTable = lua[scriptName];
				if (scriptTable.valid()) {
					if (scriptTable["on_update"].valid()) {
						script.update = scriptTable["on_update"];
					}
					if (scriptTable["on_render"].valid()) {
						script.render = scriptTable["on_render"];
					}
				} else {
					BLZR_CORE_ERROR("Failed to load script table for %s",
									scriptName.c_str());
				}
			} else {
				sol::error err = result;
				BLZR_CORE_ERROR("Error loading script: %s", err.what());
			}
		} catch (const std::exception &e) {
			BLZR_CORE_ERROR("Exception while reloading script: %s", e.what());
		}
	}

	cursorPos.y += 35;
}
void Editor::renderTransformComponent(ImVec2 &cursorPos,
									  TransformComponent &transform) {

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
	ImGui::InputFloat("##PositionX", &transform.position.x, 5, 10, "%.1f");
	if (transform.position.x > 1280) {
		transform.position.x = 1280;
	}
	if (transform.position.x < 0) {
		transform.position.x = 0;
	}
	ImGui::SameLine();
	ImGui::Text("Y");
	ImGui::SameLine();
	ImGui::InputFloat("##PositionY", &transform.position.y, 5, 10, "%.1f");
	if (transform.position.y > 720) {
		transform.position.y = 720;
	}
	if (transform.position.y < 0) {
		transform.position.y = 0;
	}

	posX = transform.position.x;
	posY = transform.position.y;
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
	ImGui::InputFloat("##ScaleX", &transform.scale.x, 0.2f, 1.f, "%.1f");
	if (transform.scale.x < 0.1f) {
		transform.scale.x = 0.1f;
	}
	if (transform.scale.x > 10.f) {
		transform.scale.x = 10.f;
	}

	ImGui::SameLine();
	ImGui::Text("Y");
	ImGui::SameLine();
	ImGui::InputFloat("##ScaleY", &transform.scale.y, 0.2f, 1.f, "%.1f");
	if (transform.scale.y < 0.1f) {
		transform.scale.y = 0.1f;
	}
	if (transform.scale.y > 10.f) {
		transform.scale.y = 10.f;
	}
	scaleY = transform.scale.y;
	scaleX = transform.scale.x;

	ImGui::PopItemWidth();
	cursorPos.y += 30;

	// Rotacija (Rotation)
	ImGui::SetCursorPos(cursorPos);
	ImGui::Text("Rotation");
	cursorPos.x += 15;
	cursorPos.y += 20;
	ImGui::SetCursorPos(cursorPos);
	ImGui::InputFloat("##Rotation", &transform.rotation, 1.0f, 10.0f, "%.1f");
	cursorPos.x -= 15;
	cursorPos.y += 35;

	if ((posX != newPosX || posY != newPosY || scaleY != newScaleY ||
		 scaleX != newScaleX) &&
		glfwGetKey(m_Window->GetWindow(), GLFW_KEY_ENTER) == GLFW_PRESS) {
		newPosX = posX;
		newPosY = posY;
		newScaleX = scaleX;
		newScaleY = scaleY;
	}
}

void Editor::renderIdentificationComponent(ImVec2 &cursorPos,
										   Identification &identification) {

	if (name != identification.name || groupName != identification.group) {
		std::copy(identification.name.begin(), identification.name.end(), name);
		name[identification.name.size()] = '\0';
		std::copy(identification.group.begin(), identification.group.end(),
				  groupName);
		name[identification.group.size()] = '\0';
	}

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
	ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
	ImGui::InputText("###group", groupName, IM_ARRAYSIZE(groupName));
	ImGui::PopItemWidth();
	cursorPos.y += 35;

	if ((name != identification.name || groupName != identification.group) &&
		glfwGetKey(m_Window->GetWindow(), GLFW_KEY_ENTER) == GLFW_PRESS) {
		BLZR_CORE_INFO("Name changed from {0} to {1}", identification.name,
					   name);
		BLZR_CORE_INFO("Group changed from {0} to {1}", identification.group,
					   groupName);

		identification.name = name;
		identification.group = groupName;
	}
}

void Editor::renderSpriteComponent(ImVec2 &cursorPos, SpriteComponent &sprite) {
	std::copy(sprite.layer.begin(), sprite.layer.end(), layer);
	name[sprite.layer.size()] = '\0';

	ImGui::SetCursorPos(cursorPos);
	ImGui::Separator();
	ImGui::Text("Sprite");
	cursorPos.y += 30;
	ImGui::SetCursorPos(cursorPos);
	ImGui::Text("Textures");
	cursorPos.y -= 3;
	cursorPos.x += 65;
	ImGui::SetCursorPos(cursorPos);
	auto assetManager = AssetManager::GetInstance();
	std::map<std::string, Ref<Texture2D>> loadedTexture =
		assetManager->getAllTextures();

	// Kreiraj vektor stringova za čuvanje svih ključeva iz mape
	std::vector<std::string> textures;

	// Iteriraj kroz mapu i dodaj sve ključeve u vektor
	for (const auto &pair : loadedTexture) {
		textures.push_back(pair.first);
	}

	static int selectedTextureIndex = -1;

	// Proveri da li ima tekstura u vektoru
	if (!textures.empty()) {
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

		// Odredi ime koje će se prikazati u Combo kada ništa nije
		// selektovano
		const char *currentTexture =
			selectedTextureIndex == -1 ? "Choose a texture"
									   : textures[selectedTextureIndex].c_str();

		// Dropdown meni
		if (ImGui::BeginCombo("##TexturesDropdown", currentTexture)) {
			for (int i = 0; i < textures.size(); i++) {
				bool isSelected = (selectedTextureIndex == i);

				// Selektovanje teksture
				if (ImGui::Selectable(textures[i].c_str(), isSelected)) {
					selectedTextureIndex = i;
					sprite.texturePath = textures[i];
				}

				// Fokusiraj trenutno selektovani element
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
	} else {
		ImGui::Text("No textures available");
	}
	// spriteWidth
	cursorPos.x -= 64;
	cursorPos.y += 28;
	ImGui::SetCursorPos(cursorPos);
	ImGui::Text("Width");
	cursorPos.x += 65;
	cursorPos.y -= 3;
	ImGui::SetCursorPos(cursorPos);
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	ImGui::InputFloat("##spriteWidth", &sprite.width, 0.1f, 1.0f, "%.1f");
	// spriteHeight
	cursorPos.x -= 65;
	cursorPos.y += 28;
	ImGui::SetCursorPos(cursorPos);
	ImGui::Text("Height");
	cursorPos.x += 65;
	cursorPos.y -= 3;
	ImGui::SetCursorPos(cursorPos);
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	ImGui::InputFloat("##spriteHeight", &sprite.height, 0.1f, 1.0f, "%.1f");
	// layer
	cursorPos.x -= 65;
	cursorPos.y += 28;
	ImGui::SetCursorPos(cursorPos);
	ImGui::Text("Layer");
	cursorPos.x += 65;
	cursorPos.y -= 3;
	ImGui::SetCursorPos(cursorPos);
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	ImGui::InputText("##layer", layer, IM_ARRAYSIZE(layer));

	cursorPos.x -= 65;
	cursorPos.y += 35;
}
void Editor::renderAnimationComponent(ImVec2 &cursorPos,
									  AnimationComponent &animation) {

	ImGui::SetCursorPos(cursorPos);
	ImGui::Separator();
	ImGui::Text("Animation");
	// numberr of frames
	cursorPos.y += 30;
	ImGui::SetCursorPos(cursorPos);
	ImGui::Text("Number of frames");
	ImGui::SameLine(ImGui::GetContentRegionAvail().x - 120);
	ImGui::PushItemWidth(130);
	ImGui::InputInt("##numFrames", &animation.numFrames);
	// frame rate
	ImGui::Text("Frame Rate");
	ImGui::SameLine(ImGui::GetContentRegionAvail().x - 120);
	ImGui::PushItemWidth(130);
	ImGui::InputInt("##frameRate", &animation.frameRate);
	ImGui::PopItemWidth();
	// frame offset
	ImGui::Text("Frame Offset");
	ImGui::SameLine(ImGui::GetContentRegionAvail().x - 120);
	ImGui::PushItemWidth(130);
	ImGui::InputInt("##frameOffset", &animation.frameOffset);
	ImGui::PopItemWidth();
	// bVertical
	ImGui::Text("bVertical");
	ImGui::SameLine(ImGui::GetContentRegionAvail().x - 10);
	ImGui::Checkbox("##bVertical", &animation.bVertical);

	cursorPos.y += 100;
}
void Editor::renderBoxColliderComponent(ImVec2 &cursorPos,
										BoxColliderComponent &boxCollider) {

	ImGui::SetCursorPos(cursorPos);
	ImGui::Separator();
	ImGui::Text("BoxCollider");
	cursorPos.y += 28;
	ImGui::SetCursorPos(cursorPos);
	ImGui::Text("Width");
	// ImGui::SameLine();
	ImGui::SameLine(ImGui::GetContentRegionAvail().x - 165);
	ImGui::InputInt("##widthBoxCollider", &widthBoxCollider);
	ImGui::Text("Height");
	ImGui::SameLine(ImGui::GetContentRegionAvail().x - 165);
	ImGui::InputInt("##heightBoxCollider", &heightBoxCollider);

	if (widthBoxCollider < 16) {
		widthBoxCollider = 16;
	}
	if (heightBoxCollider < 16) {
		heightBoxCollider = 16;
	}

	boxCollider.width = widthBoxCollider;
	boxCollider.height = heightBoxCollider;

	ImGui::Text("OffsetX");
	ImGui::SameLine(ImGui::GetContentRegionAvail().x - 165);
	ImGui::InputFloat("##offsetX", &offsetX, 0.1f, 1.0f, "%.1f");
	ImGui::Text("OffsetY");
	ImGui::SameLine(ImGui::GetContentRegionAvail().x - 165);
	ImGui::InputFloat("##offsetY", &offsetY, 0.1f, 1.0f, "%.1f");

	boxCollider.offset = {offsetX, offsetY};

	ImGui::Text("isColliding");
	ImGui::SameLine(ImGui::GetContentRegionAvail().x - 10);
	ImGui::Checkbox("##bVertical", &boxCollider.colliding);

	cursorPos.y += 120;
}

void Editor::renderPhysicsComponent(ImVec2 &cursorPos,
									PhysicsComponent &physics) {

	auto body = physics.GetRigidBody();
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
	const char *types[] = {"Static", "Kinematic", "Dynamic"};
	static int selectedTypeIndex = body->GetType();
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	if (ImGui::BeginCombo("##TypesDropdown", selectedTypeIndex == -1
												 ? "Choose a type"
												 : types[selectedTypeIndex])) {
		for (int i = 0; i < IM_ARRAYSIZE(types); i++) {
			bool isSelected = (selectedTypeIndex == i);
			if (ImGui::Selectable(types[i], isSelected)) {
				physics.GetAttributes().type = static_cast<RigidBodyType>(i);
				body->SetType(b2BodyType(i));
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
	ImGui::InputFloat("##density", &physics.GetAttributes().density, 0.1f, 1.0f,
					  "%.1f");
	// friction
	cursorPos.x -= 90;
	cursorPos.y += 28;
	ImGui::SetCursorPos(cursorPos);
	ImGui::Text("Friction");
	cursorPos.x += 90;
	cursorPos.y -= 3;
	ImGui::SetCursorPos(cursorPos);
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	ImGui::InputFloat("##friction", &physics.GetAttributes().friction, 0.1f,
					  1.0f, "%.1f");
	// restitution
	cursorPos.x -= 90;
	cursorPos.y += 28;
	ImGui::SetCursorPos(cursorPos);
	ImGui::Text("Restitution");
	cursorPos.x += 90;
	cursorPos.y -= 3;
	ImGui::SetCursorPos(cursorPos);
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	ImGui::InputFloat("##restitution", &physics.GetAttributes().restitution,
					  0.1f, 1.0f, "%.1f");
	// gravityScale
	cursorPos.x -= 90;
	cursorPos.y += 28;
	ImGui::SetCursorPos(cursorPos);
	ImGui::Text("GravityScale");
	cursorPos.x += 90;
	cursorPos.y -= 3;
	ImGui::SetCursorPos(cursorPos);
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	ImGui::InputFloat("##gravityScale", &physics.GetAttributes().gravityScale,
					  0.1f, 1.0f, "%.1f");
	body->SetGravityScale(physics.GetAttributes().gravityScale);
	// isSensor
	cursorPos.x -= 90;
	cursorPos.y += 28;
	ImGui::SetCursorPos(cursorPos);
	ImGui::Text("isSensor");
	ImGui::SameLine(ImGui::GetContentRegionAvail().x - 12);
	ImGui::Checkbox("##isSensorCheckbox", &physics.GetAttributes().isSensor);

	// isFixedRotation
	cursorPos.y += 25;
	ImGui::SetCursorPos(cursorPos);
	ImGui::Text("isFixedRotation");
	ImGui::SameLine(ImGui::GetContentRegionAvail().x - 12);
	ImGui::Checkbox("##isFixedRotation",
					&physics.GetAttributes().isFixedRotation);

	body->SetFixedRotation(physics.GetAttributes().isFixedRotation);

	cursorPos.y += 35;
	if (CameraController::paused) {

		for (b2Fixture *fixture = body->GetFixtureList(); fixture != nullptr;
			 fixture = fixture->GetNext()) {
			fixture->SetDensity(physics.GetAttributes().density);
			fixture->SetRestitution(physics.GetAttributes().restitution);
			fixture->SetFriction(physics.GetAttributes().friction);
			fixture->SetSensor(physics.GetAttributes().isSensor);
		}
	}

	if ((posX != newPosX || posY != newPosY || scaleX != newScaleX ||
		 scaleY != newScaleY || widthBoxCollider != newWidthBoxCollider ||
		 heightBoxCollider != newHeightBoxCollider || offsetX != newOffsetX ||
		 offsetY != newOffsetY) &&
		glfwGetKey(m_Window->GetWindow(), GLFW_KEY_ENTER) == GLFW_PRESS) {
		physics.GetAttributes().position = {posX, posY};
		physics.GetAttributes().scale = {scaleX, scaleY};
		physics.GetAttributes().boxSize = {widthBoxCollider, heightBoxCollider};
		physics.GetAttributes().offset = {offsetX, offsetY};
		if (CameraController::paused) {
			physics.init(1280, 720);
		}

		newPosX = posX;
		newPosY = posY;
		newScaleX = scaleX;
		newScaleY = scaleY;
		newWidthBoxCollider = widthBoxCollider;
		newHeightBoxCollider = heightBoxCollider;
		newOffsetX = offsetX;
		newOffsetY = offsetY;
	}
}

} // namespace Blazr
