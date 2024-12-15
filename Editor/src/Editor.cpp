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
#include "utils/FileDialog.h"
// #include "utils/FileDialog.h"
#include <GLFW/glfw3.h>
#include <memory>

namespace Blazr {
// layers
static int selectedLayerIndex = 0;
static bool showAddLayerPopup = false;
static char newLayerName[256] = "New Layer";
static int newLayerZIndex = 0;
static bool layerChanged = false;

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

// sprite
// static char layer[128] = "";
static float spriteWidth = 0.0f;
static float spriteHeight = 0.0f;
static float newSpriteWidth = 0.0f;
static float newSpriteHeight = 0.0f;
static float layer = 0.0f;
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
static bool showSceneChooseDialog = false;

// managers

static auto assetManager = AssetManager::GetInstance();
static auto soundPlayer = SoundPlayer::GetInstance();
static std::string selectedTielset = "";
static std::string luaScriptContent = ""; // Sadržaj Lua skripte
static char
	luaScriptBuffer[1024 * 16]; // Buffer za unos teksta (podešen na 16KB)

static Ref<Project> newProject;
static std::string selectedScene = "";
static bool sceneIsSelected = false;
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
	m_Renderer = Renderer2D();
	InitImGui();
	newProject = Project::New("StartProject");
	Project::SetActive(newProject);
	Ref<Scene> newScene = CreateRef<Scene>();
	newScene->SetName("Untitled 1");
	newProject->AddScene("Untitled 1", newScene);

	m_ActiveScene = newScene;

	ProjectSerializer::Serialize(newProject, newProject->GetProjectDirectory() /
												 newProject->GetConfig().name);

	if (!m_ScriptSystem->LoadMainScript(*m_LuaState)) {
		BLZR_CORE_ERROR("Failed to load the main lua script");
		return;
	}
	// TODO should also initialize entity script when a new entity is created
	// m_ScriptSystem->InitializeEntityScripts(*m_LuaState);
	//
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
	glfwSetCursorPosCallback(
		m_Window->GetWindow(),
		[](GLFWwindow *window, double xPos, double yPos) {
			auto eventCallback = static_cast<std::function<void(Event &)> *>(
				glfwGetWindowUserPointer(window));

			if (eventCallback) {
				MouseMovedEvent event(static_cast<float>(xPos),
									  static_cast<float>(yPos));
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

static std::string selectedGameObject = "GameObject";
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
				std::string savePath = Blazr::FileDialog::SaveFile(
					"Blazr Project (*.blzr)\0*.blzr\0");
				if (!savePath.empty()) {
					Registry::Reset();
					Initialize();
					Ref<Project> newProject = Project::New("UntitledProject");
					Ref<Scene> newScene = CreateRef<Scene>();
					newScene->SetName("Untitled 1");
					newProject->AddScene("Untitled 1", newScene);
					newProject->GetConfig().StartSceneName = "Untitled 1";
					m_ActiveScene = newScene;

					newProject->SetProjectDirectory(
						std::filesystem::path(savePath).parent_path());
					newProject->GetConfig().name =
						std::filesystem::path(savePath).stem().string();

					ProjectSerializer::Serialize(newProject, savePath);
				}
			}

			if (ImGui::MenuItem("Open...")) {
				std::string openPath = Blazr::FileDialog::OpenFile(
					"Blazr Project (*.blzr)\0*.blzr\0");
				if (!openPath.empty()) {
					Registry::Reset();
					Initialize();
					Ref<Project> loadedProject = Project::Load(openPath);
					if (loadedProject) {
						Project::SetActive(loadedProject);
						m_ActiveScene = loadedProject->GetScene(
							loadedProject->GetConfig().StartSceneName);
						BLZR_CORE_ERROR(
							"START SCENE: {0}",
							loadedProject->GetConfig().StartSceneName);
					} else {
						BLZR_CORE_ERROR("Failed to load project from: {}",
										openPath);
					}
				}
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
			if (ImGui::MenuItem("Export tilemap to scene")) {
				showSceneChooseDialog = true;
				// auto tilemapScene =
				// 	std::dynamic_pointer_cast<TilemapScene>(m_ActiveScene);
				// if (tilemapScene) {
				// 	for (const auto &[name, scene] :
				// 		 Project::GetActive()->GetScenes()) {
				// 		BLZR_CORE_INFO("Scene name: {0}", scene->GetName());
				// 		if (scene->GetName() == "Untitled 1") {
				// 			tilemapScene->ExportTilemapToScene(*scene);
				// 		}
				// 	}
				// }
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
			ImGui::MenuItem("Show Collider", nullptr, &Layer::showColliders);
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

	if (showSceneChooseDialog) {
		ImVec2 windowSize(300, 300);
		ImVec2 windowPos(100, 100);
		ImGui::SetNextWindowSize(windowSize);
		ImGui::SetNextWindowPos(windowPos);
		if (ImGui::Begin("Choose Scene Dialog", nullptr,
						 ImGuiWindowFlags_NoCollapse)) {
			auto scenes = newProject->GetScenes();
			if (ImGui::BeginCombo("Select item", selectedScene.c_str())) {
				for (const auto &scene : scenes) {
					const bool isSelected = (selectedScene == scene.first);

					if (ImGui::Selectable(scene.first.c_str(), isSelected)) {
						selectedScene = scene.first;
						showSceneChooseDialog = false;
					}

					if (isSelected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			auto tilemapScene =
				std::dynamic_pointer_cast<TilemapScene>(m_ActiveScene);
			if (tilemapScene) {

				for (auto pair : scenes) {
					if (pair.first == selectedScene) {
						tilemapScene->ExportTilemapToScene(*pair.second);
						selectedScene = "";
					}
				}
			}
		}
		ImGui::End();
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
	if (ImGui::BeginPopupContextWindow("AddGameObject",
									   ImGuiPopupFlags_MouseButtonRight)) {
		if (ImGui::MenuItem("AddGameObject")) {
			Ref<Entity> ent = CreateRef<Entity>(*m_Registry);
			auto l = m_ActiveScene->GetAllLayers().at(0);
			l->AddEntity(ent);
		}
		ImGui::EndPopup();
	}

	auto tilemapScene = std::dynamic_pointer_cast<TilemapScene>(m_ActiveScene);

	if (!tilemapScene) {

		for (Ref<Layer> layer : m_ActiveScene->GetAllLayers()) {
			for (Ref<Entity> ent : layer->entities) {
				if (!ent->HasComponent<TileComponent>()) {
					std::string gameObjectName =
						ent->GetComponent<Identification>().name;

					if (ImGui::Selectable(gameObjectName.c_str())) {
						selectedGameObject = gameObjectName;
						showGameObjectDetails = true;
					}
				}
			}
		}
	}

	auto view = m_Registry->GetRegistry().view<entt::entity>(
		entt::exclude<TileComponent>);
	//
	// for (auto entity : view) {
	// 	auto &identification =
	// 		m_Registry->GetRegistry().get<Identification>(entity);
	// 	std::string gameObjectName = identification.name;
	// 	if (ImGui::Selectable(gameObjectName.c_str())) {
	// 		selectedGameObject = "ObjectDetails-" + gameObjectName;
	// 		showGameObjectDetails = true;
	// 	}
	// }
	ImVec2 scenePos = ImGui::GetWindowPos();
	ImVec2 sceneSize = ImGui::GetWindowSize();
	ImGui::End();

	//---------------------------------------------------------------2. box
	//-
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
				if (!tilemapScene) {

					for (Ref<Layer> layer : m_ActiveScene->GetAllLayers()) {
						for (Ref<Entity> ent : layer->entities) {

							if (ent->HasComponent<TileComponent>()) {
								continue;
							}

							entt::entity &entity = ent->GetEntityHandler();

							std::string entityName =
								m_Registry->GetRegistry()
									.get<Identification>(entity)
									.name;
							if (m_Registry->GetRegistry()
									.all_of<TransformComponent>(entity) &&
								selectedGameObject == entityName) {
								auto &transform =
									m_Registry->GetRegistry()
										.get<TransformComponent>(entity);

								renderTransformComponent(cursorPos, transform);
							}
							if (m_Registry->GetRegistry()
									.all_of<SpriteComponent>(entity) &&
								selectedGameObject == entityName) {
								auto &sprite =
									m_Registry->GetRegistry()
										.get<SpriteComponent>(entity);
								showColorTab = true;

								auto &identification =
									m_Registry->GetRegistry()
										.get<Identification>(entity);
								renderSpriteComponent(cursorPos, sprite,
													  identification);

								std::vector<Ref<Layer>> layers =
									m_ActiveScene->GetLayerManager()
										->GetAllLayers();
								auto l =
									m_ActiveScene->GetLayerByName(sprite.layer);
								if (l && layerChanged) {
									l->AddEntity(
										CreateRef<Entity>(*m_Registry, entity));
									layerChanged = false;
								}
							}
							if (m_Registry->GetRegistry()
									.all_of<PhysicsComponent>(entity) &&
								selectedGameObject == entityName) {
								auto &physics =
									m_Registry->GetRegistry()
										.get<PhysicsComponent>(entity);
								renderPhysicsComponent(cursorPos, physics);
							}

							if (m_Registry->GetRegistry()
									.all_of<AnimationComponent>(entity) &&
								selectedGameObject == entityName) {
								auto &animation =
									m_Registry->GetRegistry()
										.get<AnimationComponent>(entity);
								renderAnimationComponent(cursorPos, animation);
							}
							if (m_Registry->GetRegistry()
									.all_of<BoxColliderComponent>(entity) &&
								selectedGameObject == entityName) {
								auto &boxCollider =
									m_Registry->GetRegistry()
										.get<BoxColliderComponent>(entity);
								renderBoxColliderComponent(cursorPos,
														   boxCollider);
							}
							if (m_Registry->GetRegistry()
									.all_of<Identification>(entity) &&
								selectedGameObject == entityName) {
								auto &identification =
									m_Registry->GetRegistry()
										.get<Identification>(entity);
								renderIdentificationComponent(cursorPos,
															  identification);
							}

							if (m_Registry->GetRegistry()
									.all_of<ScriptComponent>(entity) &&
								selectedGameObject == entityName) {
								auto &script =
									m_Registry->GetRegistry()
										.get<ScriptComponent>(entity);
								renderScriptComponent(cursorPos, script,
													  entity);
							}
						}
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

						auto view =
							m_Registry->GetRegistry().view<entt::entity>(
								entt::exclude<TileComponent>);
						for (auto entity : view) {
							std::string entityName =
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
	RenderSceneControls(showCodeEditor, luaScriptContent, luaScriptBuffer,
						newProject);
	RenderActiveScene(m_ActiveScene);

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
			// TO DO
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Tileset")) {

			auto tilesets = assetManager->GetKeysTexturesTileset();
			// BLZR_CORE_ERROR("sizeeeeeeeeeeeeeeeeeeeeee {0}",
			// tilesets.size());
			if (ImGui::BeginCombo("Choose Tileset", selectedTielset.c_str())) {
				for (const auto &tileset : tilesets) {
					bool bIsSelected = selectedTielset == tileset;
					if (ImGui::Selectable(tileset.c_str(), bIsSelected)) {
						selectedTielset = tileset;
					}
					if (bIsSelected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			auto texture = assetManager->GetTexture(selectedTielset);
			if (texture) {
				double tileWidth = 16.f;
				double tileHeight = 16.f;
				int textureWidth = texture->GetWidth();
				int textureHeight = texture->GetHeight();
				int columns = textureWidth / tileWidth;
				int rows = textureHeight / tileHeight;
				int k = 0;
				int id = 0;
				ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

				for (int row = 0; row < rows; row++) {
					for (int col = 0; col < columns; col++) {
						ImVec2 uv0((float)col * tileWidth / (float)textureWidth,
								   1.0f - (float)(row * tileHeight) /
											  (float)textureHeight); // Flip Y
						ImVec2 uv1((float)(col + 1) * tileWidth /
									   (float)textureWidth,
								   1.0f - (float)((row + 1) * tileHeight) /
											  (float)textureHeight);
						ImGui::PushID(k++);
						if (ImGui::ImageButton(
								"",
								(ImTextureID)(intptr_t)texture->GetRendererID(),
								ImVec2(tileWidth, tileHeight), uv0, uv1)) {

							auto tilemapScene =
								std::dynamic_pointer_cast<TilemapScene>(
									m_ActiveScene);

							if (tilemapScene) {
								tilemapScene->SetSelectedTile(
									std::pair<std::string, glm::vec3>(
										selectedTielset,
										glm::vec3(col, rows, row)));
							}

							// BLZR_CORE_ERROR("{0} {1}", row, col);
						}
						ImGui::PopID();
						ImGui::SameLine();
					}
					ImGui::NewLine();
				}
				ImGui::PopStyleVar(2);
			}
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
	ImGui::SetCursorPos(cursorPos);
	ImGui::Separator();
	ImGui::Text("Identification");
	cursorPos.y += 30;

	ImGui::SetCursorPos(cursorPos);
	ImGui::Text("Name");
	cursorPos.x += 43;
	ImGui::SetCursorPos(cursorPos);
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

	char nameBuffer[256];
	strncpy(nameBuffer, identification.name.c_str(), sizeof(nameBuffer) - 1);
	nameBuffer[sizeof(nameBuffer) - 1] = '\0';

	if (ImGui::InputText("###nameObject", nameBuffer, sizeof(nameBuffer),
						 ImGuiInputTextFlags_EnterReturnsTrue)) {
		identification.name = nameBuffer;
	}

	cursorPos.x -= 43;
	cursorPos.y += 25;
	ImGui::SetCursorPos(cursorPos);
	ImGui::Text("Group");
	ImGui::SameLine();
	ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
	ImGui::InputText("###group", identification.group.data(),
					 IM_ARRAYSIZE(identification.group.data()));
	ImGui::PopItemWidth();
	cursorPos.y += 35;
}

void Editor::renderSpriteComponent(ImVec2 &cursorPos, SpriteComponent &sprite,
								   Identification &identification) {

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

	if ((sprite.width != spriteWidth || sprite.height != spriteHeight) &&
		glfwGetKey(m_Window->GetWindow(), GLFW_KEY_ENTER) == GLFW_PRESS) {
		spriteWidth = sprite.width;
		spriteHeight = sprite.height;
		sprite.generateObject(sprite.width, sprite.height);
	}
	// layer
	cursorPos.x -= 65;
	cursorPos.y += 28;
	ImGui::SetCursorPos(cursorPos);

	ImGui::Text("Layer");
	cursorPos.x += 65;
	cursorPos.y -= 3;
	ImGui::SetCursorPos(cursorPos);
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

	static std::unordered_map<SpriteComponent *, int>
		previousSelectedLayerIndices;

	std::vector<Ref<Layer>> layers =
		m_ActiveScene->GetLayerManager()->GetAllLayers();

	// Ensure the sprite has a valid current layer and selectedLayerIndex
	if (!sprite.layer.empty()) {
		for (int i = 0; i < layers.size(); i++) {
			if (layers[i]->name == sprite.layer) {
				selectedLayerIndex = i;
				break;
			}
		}
	} else {
		sprite.layer = layers[0]->name;
		selectedLayerIndex = 0;
	}

	int &previousSelectedLayerIndex = previousSelectedLayerIndices[&sprite];
	if (previousSelectedLayerIndex == 0 && selectedLayerIndex != 0) {
		previousSelectedLayerIndex = selectedLayerIndex;
	}

	if (ImGui::BeginCombo("##LayerDropdown",
						  selectedLayerIndex == -1
							  ? "Select a layer:"
							  : layers[selectedLayerIndex]->name.c_str())) {

		if (ImGui::Selectable("Add New Layer", false)) {
			showAddLayerPopup = true;
			strncpy(newLayerName, "New Layer", sizeof(newLayerName));
			newLayerZIndex = 0;
		}

		for (int i = 0; i < layers.size(); i++) {
			bool isSelected = (selectedLayerIndex == i);
			if (ImGui::Selectable(layers[i]->name.c_str(), isSelected)) {
				if (previousSelectedLayerIndex != i) {
					previousSelectedLayerIndex = i;
					selectedLayerIndex = i;
					m_ActiveScene->GetLayerByName(sprite.layer)
						->RemoveEntity(identification.name);
					sprite.layer = layers[i]->name;
					layerChanged = true;
				}
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	if (showAddLayerPopup) {
		ImGui::OpenPopup("Create New Layer");
	}

	if (ImGui::BeginPopupModal("Create New Layer", &showAddLayerPopup,
							   ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Enter Layer Details:");
		ImGui::Separator();

		ImGui::InputText("Layer Name", newLayerName, sizeof(newLayerName));
		ImGui::InputInt("zIndex", &newLayerZIndex);

		if (ImGui::Button("Create", ImVec2(120, 0))) {
			Ref<Layer> newLayer =
				CreateRef<Layer>(newLayerName, newLayerZIndex);
			m_ActiveScene->GetLayerManager()->AddLayer(newLayer);

			layers = m_ActiveScene->GetLayerManager()->GetAllLayers();
			selectedLayerIndex = layers.size() - 1;

			showAddLayerPopup = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) {
			showAddLayerPopup = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	cursorPos.x -= 65;
	cursorPos.y += 35;
}

void Editor::renderScriptComponent(ImVec2 &cursorPos, ScriptComponent &script,
								   const entt::entity &entity) {
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

	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	ImGui::Text(const_cast<char *>(script.scriptPath.c_str()));

	cursorPos.y += 25; // Adjust cursor position for buttons

	// Create Script Button
	if (ImGui::Button("Create Script")) {
		std::filesystem::path path = Project::GetProjectDirectory() / "scripts";
		if (!std::filesystem::exists(path)) {
			if (!std::filesystem::create_directory(path)) {
				BLZR_CORE_ERROR("Error creating script folder");
				return;
			}
		}
		std::string absPath = std::filesystem::absolute(path);

		std::string newScriptPath = FileDialog::SaveFileWithPath(
			"Lua Scripts (*.lua)\0*.lua\0", absPath.c_str());

		if (!newScriptPath.empty()) {
			std::string defaultScript = "-- Default Lua Script\n"
										"\n"
										"local script = {}\n"
										"\n"
										"function script.on_update(entity)\n"
										"    -- Your update logic here\n"
										"    print(\"hello world from lua\")\n"
										"end\n"
										"\n"
										"function script.on_render(entity)\n"
										"    -- Your render logic here\n"
										"end\n"
										"\n"
										"return script\n";

			std::ofstream outFile(newScriptPath + ".lua");
			if (outFile.is_open()) {
				outFile << defaultScript;
				outFile.close();

				script.scriptPath = newScriptPath + ".lua";
				auto &identification =
					m_Registry->GetRegistry().get<Identification>(entity);
				Ref<Entity> ent = CreateRef<Entity>(*m_Registry, entity);
				script.LoadScript(*m_LuaState, ent, identification.name);

				ImGui::OpenPopup("Script Created");
			} else {
				ImGui::OpenPopup("Error Creating Script");
			}
		}
	}

	// Load Script Button
	if (ImGui::Button("Load Script")) {
		std::filesystem::path path = Project::GetProjectDirectory() / "scripts";
		std::string absPath = std::filesystem::absolute(path);

		std::string newScriptPath = FileDialog::OpenFileWithPath(
			"Lua Scripts (*.lua)\0*.lua\0", absPath.c_str());

		if (!newScriptPath.empty()) {
			script.update = sol::nil;
			script.render = sol::nil;

			script.scriptPath = newScriptPath;

			auto &identification =
				m_Registry->GetRegistry().get<Identification>(entity);
			Ref<Entity> ent = CreateRef<Entity>(*m_Registry, entity);
			script.LoadScript(*m_LuaState, ent, identification.name);

			ImGui::OpenPopup("Script Loaded");
		} else {
			ImGui::OpenPopup("Error Loading Script");
		}
	}

	// Popups for success/failure
	// if (ImGui::BeginPopup("Script Created")) {
	// 	ImGui::Text("Script successfully created!");
	// 	ImGui::EndPopup();
	// }

	// if (ImGui::BeginPopup("Error Creating Script")) {
	// 	ImGui::Text("Failed to create the script file.");
	// 	ImGui::EndPopup();
	// }
	//
	// if (ImGui::BeginPopup("Script Loaded")) {
	// 	ImGui::Text("Script successfully loaded!");
	// 	ImGui::EndPopup();
	// }
	//
	// if (ImGui::BeginPopup("Error Loading Script")) {
	// 	ImGui::Text("Failed to load the script file.");
	// 	ImGui::EndPopup();
	// }

	cursorPos.y += 35; // Adjust cursor position for layout
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
	BLZR_CORE_INFO("Type: {0}", selectedTypeIndex);
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	if (ImGui::BeginCombo("##TypesDropdown", selectedTypeIndex == -1
												 ? "Choose a type"
												 : types[selectedTypeIndex])) {
		for (int i = 0; i < IM_ARRAYSIZE(types); i++) {
			bool isSelected = (selectedTypeIndex == i);
			if (ImGui::Selectable(types[i], isSelected)) {
				selectedTypeIndex = i;
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
