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
#include "Blazr/Renderer/FollowCamera.h"
#include "Blazr/Renderer/Renderer2D.h"
#include "Blazr/Resources/AssetManager.h"
#include "Blazr/Scene/TilemapScene.h"
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
namespace fs = std::filesystem;
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
static int startX = 0.0f;
static int startY = 0.0f;

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
static float physicsPosX = 0.0f;
static float physicsPosY = 0.0f;
// za GUI
static bool showCodeEditor = false;
static bool showTransformComponent = false;
static bool showIdentificationComponent = false;
static bool showSpriteComponent = false;
static bool showPhysicsComponent = false;
static bool showColorTab = false;
static bool showSceneChooseDialog = false;
static int selectedTilemapLayer = -1;

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
static int canvasWidth = 16;
static int canvasHeight = 16;
static int tilemapScaleX = 1;
static int tilemapScaleY = 1;
static std::string identificationGroup = "";

bool Editor::clickedStop = false;

Editor::Editor() { Init(); }

Editor::~Editor() { Shutdown(); }

void Editor::Init() {
	BLZR_CORE_INFO("Initializing Editor...");

	Renderer2D::Init();
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
	newProject->GetConfig().StartSceneName = "Untitled 1";
	newProject->AddScene("Untitled 1", newScene);

	m_ActiveScene = newScene;

	ProjectSerializer::Serialize(newProject, newProject->GetProjectDirectory() /
												 newProject->GetConfig().name);

	// if (!m_ScriptSystem->LoadMainScript(*m_LuaState)) {
	// 	BLZR_CORE_ERROR("Failed to load the main lua script");
	// 	return;
	// }
	assetManager->LoadTexture("default", "assets/white_texture.png");
	assetManager->LoadTexture("collider", "assets/collider.png");
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
	}

	glfwSwapBuffers(m_Window->GetWindow());
}

static std::string selectedGameObject = "GameObject";
void Editor::RenderImGui() {
	ImVec2 cursorPos = ImVec2(10, 55);
	ImVec2 tilemapCursorPos = ImVec2(10, 55);
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
	static bool showTilemapSettings = false;
	static bool showTileObjectDetails = false;
	int numberOfComponents = 3;

	// Menu bar
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("New Project")) {
				std::string savePath = Blazr::FileDialog::SaveFile(
					"Blazr Project (*.blzrproj)\0*.blzrproj\0");
				if (!savePath.empty()) {
					Registry::Reset();
					Initialize();

					Ref<Project> newProject = Project::New("UntitledProject");
					Ref<Scene> newScene = CreateRef<Scene>();
					newScene->SetName("Untitled 1");
					newProject->AddScene("Untitled 1", newScene);
					newProject->GetConfig().StartSceneName = "Untitled 1";
					m_ActiveScene = newScene;

					std::filesystem::path projectDir =
						std::filesystem::path(savePath).parent_path();
					newProject->SetProjectDirectory(projectDir);
					newProject->GetConfig().name =
						std::filesystem::path(savePath).stem().string();

					ProjectSerializer::Serialize(newProject, savePath);

					std::filesystem::path assetsSrcDir = "assets";
					std::filesystem::path shadersSrcDir = "shaders";

					if (std::filesystem::exists(assetsSrcDir)) {
						std::filesystem::path assetsTargetDir =
							projectDir / "assets";
						std::filesystem::copy(
							assetsSrcDir, assetsTargetDir,
							std::filesystem::copy_options::recursive);
					} else {
						BLZR_CORE_WARN(
							"Assets folder not found in source directory: {}",
							assetsSrcDir.string());
					}

					if (std::filesystem::exists(shadersSrcDir)) {
						std::filesystem::path shadersTargetDir =
							projectDir / "shaders";
						std::filesystem::copy(
							shadersSrcDir, shadersTargetDir,
							std::filesystem::copy_options::recursive);
					} else {
						BLZR_CORE_WARN(
							"Shaders folder not found in source directory: {}",
							shadersSrcDir.string());
					}
				}
			}

			if (ImGui::MenuItem("Open...")) {
				std::string openPath = Blazr::FileDialog::OpenFile(
					"Blazr Project (*.blzr)\0*.blzr\0");
				if (!openPath.empty()) {
					// Load the project and get its stored directory
					Ref<Project> loadedProject =
						Project::Load(openPath, m_LuaState);
					if (loadedProject) {
						std::string projectDirFromFile =
							loadedProject->GetProjectDirectory();

						// Extract the directory from the open path
						std::string openDir =
							fs::path(openPath).parent_path().string();

						if (projectDirFromFile != openDir) {
							BLZR_CORE_WARN(
								"Project directory mismatch! Updating project "
								"directory from: {} to: {}",
								projectDirFromFile, openDir);
							loadedProject->SetProjectDirectory(openDir);
						}

						Project::SetActive(loadedProject);
						m_ActiveScene = loadedProject->GetScene(
							loadedProject->GetConfig().StartSceneName);
					} else {
						BLZR_CORE_ERROR("Failed to load project from: {}",
										openPath);
					}
				}
			}

			if (ImGui::MenuItem("Open tileset...")) {
				std::string filepath = Blazr::FileDialog::OpenFile(
					"Image Files\0*.png;*.jpg;*.jpeg\0\0");
				if (!filepath.empty()) {
					std::string projectDir =
						Project::GetProjectDirectory().string();
					std::string assetsDir = projectDir + "/assets";

					std::string subfolder = "/textures";

					std::string targetDir = assetsDir + subfolder;

					if (!fs::exists(targetDir)) {
						fs::create_directories(targetDir);
					}

					std::string filename =
						fs::path(filepath).filename().string();

					std::string relativePath =
						"assets" + subfolder + "/" + filename;

					std::string targetPath = projectDir + "/" + relativePath;

					try {
						fs::copy(filepath, targetPath,
								 fs::copy_options::overwrite_existing);

						auto &assetManager = Blazr::AssetManager::GetInstance();
						bool success = assetManager->LoadTexture(
							filename, relativePath, true, true);
						if (success) {
							ImGui::Text("Imported and Loaded Asset: %s",
										filename.c_str());
						} else {
							ImGui::Text("Failed to load asset: %s",
										filename.c_str());
						}
					} catch (const fs::filesystem_error &e) {
						ImGui::Text("Error copying file: %s", e.what());
					}
				} else {
					ImGui::Text("No file selected.");
				}
			}

			if (ImGui::MenuItem("Save As...")) {
				std::string savePath =
					Project::GetActive()->GetProjectDirectory().string();
				BLZR_CORE_ERROR("ACTIVE DIR: {0}", savePath);
				if (!savePath.empty()) {

					Ref<Project> active = Project::GetActive();
					if (active) {
						ProjectSerializer::Serialize(active, savePath);
					} else {
						BLZR_CORE_ERROR("No active project to save.");
					}
				}
			}
			if (ImGui::MenuItem("Export tilemap to scene")) {
				showSceneChooseDialog = true;
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

			if (ImGui::MenuItem("Clear scene")) {

				for (auto &scene : newProject->GetScenes()) {
					auto tilemapScene =
						std::dynamic_pointer_cast<TilemapScene>(scene.second);
					if (tilemapScene) {
						if (std::dynamic_pointer_cast<TilemapScene>(
								m_ActiveScene) == nullptr) {
							tilemapScene->clearScene(*m_ActiveScene);
							break;
						} else {
							for (auto &scene : newProject->GetScenes()) {
								if (tilemapScene) {
									if (std::dynamic_pointer_cast<TilemapScene>(
											scene.second) == nullptr)
										tilemapScene->clearScene(*scene.second);
								}

								tilemapScene->clearRegistry();
							}
						}
					}
				}
			}
			ImGui::EndMenu();
		}
#include <filesystem>
#include <iostream>
#include <vector>

		namespace fs = std::filesystem;

		if (ImGui::BeginMenu("Tools")) {
			ImGui::MenuItem("Show Collider", nullptr, &Layer::showColliders);

			if (ImGui::MenuItem("Build Game")) {
				std::string outputDir =
					Blazr::FileDialog::OpenFolderWithPath(".");
				if (!outputDir.empty()) {
					try {
						std::string executableDir;
#ifdef _WIN32
#ifdef NDEBUG
						executableDir = "bin/Release-windows-x86_64/Sandbox";
#else
						executableDir = "bin/Debug-windows-x86_64/Sandbox";
#endif
#elif defined(__linux__)
#ifdef NDEBUG
						executableDir = "bin/Release-linux-x86_64/Sandbox";
#else
						executableDir = "bin/Debug-linux-x86_64/Sandbox";
#endif
#else
#error Unsupported platform!
#endif
						std::string projectDir =
							Project::GetProjectDirectory().string();
						std::vector<std::string> foldersToCopy = {
							"assets", "scenes", "scripts"};

						if (!fs::exists(outputDir)) {
							fs::create_directories(outputDir);
						}

						if (fs::exists(executableDir)) {
							std::string targetExecutableDir = outputDir;

							fs::copy(executableDir, targetExecutableDir,
									 fs::copy_options::recursive |
										 fs::copy_options::overwrite_existing);
							std::cout << "Copied executable folder to: "
									  << targetExecutableDir << std::endl;
						} else {
							std::cerr << "Executable folder not found: "
									  << executableDir << std::endl;
						}

						Project proj = *Project::GetActive();
						proj.GetConfig().name = "project";
						Ref<Project> p = CreateRef<Project>(proj);
						ProjectSerializer::Serialize(p, outputDir);

						fs::copy(projectDir + "/" +
									 Project::GetActive()->GetConfig().name +
									 ".blzrproj",
								 outputDir + "/project.blzrproj",
								 fs::copy_options::overwrite_existing);

						std::string gameOutputDir =
							outputDir + "/" + proj.GetConfig().name;
						BLZR_CORE_ERROR("DIRECTORY: {0}", gameOutputDir);

						if (!fs::exists(gameOutputDir)) {
							fs::create_directories(gameOutputDir);
						}

						fs::copy("assets", outputDir + "/" + "assets",
								 fs::copy_options::recursive |
									 fs::copy_options::overwrite_existing);

						// fs::copy("lib", outputDir,
						// 		 fs::copy_options::recursive |
						// 			 fs::copy_options::overwrite_existing);

						fs::copy("shaders", outputDir + "/" + "shaders",
								 fs::copy_options::recursive |
									 fs::copy_options::overwrite_existing);

						// Copy other project folders (assets, scenes, scripts)
						for (const auto &folder : foldersToCopy) {
							std::string sourcePath = projectDir + "/" + folder;
							std::string targetPath =
								gameOutputDir + "/" + folder;

							if (fs::exists(sourcePath)) {
								// Copy each folder and its contents recursively
								fs::copy(
									sourcePath, targetPath,
									fs::copy_options::recursive |
										fs::copy_options::overwrite_existing);
								std::cout << "Copied folder: " << folder
										  << " to " << targetPath << std::endl;
							} else {
								std::cerr << "Warning: Folder not found: "
										  << sourcePath << std::endl;
							}
						}

						std::cout << "Game build completed successfully!"
								  << std::endl;

					} catch (const fs::filesystem_error &e) {
						std::cerr << "Error during build process: " << e.what()
								  << std::endl;
					}
				} else {
					std::cout << "Build cancelled. No directory selected."
							  << std::endl;
				}
			}

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
			auto scenes = Project::GetActive()->GetScenes();
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
		if (m_ActiveScene != nullptr) {

			for (Ref<Layer> layer : m_ActiveScene->GetAllLayers()) {
				for (Ref<Entity> ent : layer->entities) {
					if (!ent->HasComponent<TileComponent>()) {
						std::string gameObjectName =
							ent->GetComponent<Identification>().name;

						if (ImGui::Selectable(gameObjectName.c_str())) {
							selectedGameObject = gameObjectName;
							showGameObjectDetails = true;
						}
						if (ImGui::BeginPopupContextItem(
								gameObjectName.c_str())) {
							if (!ent->GetFollowCamera()) {
								if (ImGui::MenuItem("Add Follower Camera")) {
									m_ActiveScene->SetFollowCamera(ent);
									ent->SetFollowCamera(true);
								}
								ImGui::EndPopup();
							} else if (ent->GetFollowCamera()) {
								if (ImGui::MenuItem("Remove Follower Camera")) {
									// Logika za dodavanje follower kamere
									m_ActiveScene->SetFollowCamera(nullptr);
									ent->SetFollowCamera(false);
								}
								ImGui::EndPopup();
							}
						}
					}
				}
			}
		}
		showTilemapSettings = false;
	} else {
		showTilemapSettings = true;
		showGameObjectDetails = false;
		std::unordered_set<std::string>
			displayedGroups; // Set za praćenje ispisanih grupa

		for (Ref<Layer> layer : m_ActiveScene->GetAllLayers()) {
			for (Ref<Entity> ent : layer->entities) {
				if (ent->HasComponent<TileComponent>() &&
					ent->HasComponent<PhysicsComponent>()) {
					std::string gameObjectName =
						ent->GetComponent<Identification>().group;

					// Proveri da li je grupa već ispisana
					if (displayedGroups.find(gameObjectName) ==
						displayedGroups.end()) {
						if (ImGui::Selectable(gameObjectName.c_str())) {
							selectedGameObject = gameObjectName;
							showTileObjectDetails = true;
						}
						displayedGroups.insert(
							gameObjectName); // Dodaj grupu u set
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

								if (!clickedStop &&
									m_Registry->GetRegistry()
										.all_of<PhysicsComponent>(entity)) {
									transform.position = {physicsPosX,
														  physicsPosY};
								}
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

								if (!clickedStop) {
									BLZR_CORE_ERROR("{0} {1}", newPosX,
													newPosY);
									// physics.GetAttributes().position = {
									// 	newPosX, newPosY};
									//
									// physics.init(1280, 720);
									const auto scaleHalfHeight =
										1224 * PIXELS_TO_METERS / 2;
									const auto scaleHalfWidth =
										648 * PIXELS_TO_METERS / 2;

									auto bx = (posX * PIXELS_TO_METERS) -
											  scaleHalfHeight;
									auto by = (posY * PIXELS_TO_METERS) -

											  scaleHalfWidth;

									physics.GetRigidBody()->SetTransform(
										b2Vec2{bx, by}, 0.f);

									clickedStop = true;
								}
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
								if (m_Registry->GetRegistry()
										.all_of<SpriteComponent>(entity)) {
									auto &sprite =
										m_Registry->GetRegistry()
											.get<SpriteComponent>(entity);
									renderIdentificationComponent(
										cursorPos, identification,
										sprite.layer);
								} else {
									renderIdentificationComponent(
										cursorPos, identification, "");
								}
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
	} else if (showTilemapSettings) {
		if (ImGui::BeginTabBar("Tilemap Settings")) {
			if (ImGui::BeginTabItem("TileMap Settings")) {
				if (!CameraController::paused) {
					ImGui::BeginDisabled(true);
				}

				renderTileMapSettings(tilemapCursorPos, *tilemapScene);

				auto view = m_Registry->GetRegistry()
								.view<PhysicsComponent, TileComponent,
									  Identification, SpriteComponent>();
				bool physicsComponentRendered =
					false; // Flag za prikaz PhysicsComponent samo jednom
				std::map<std::string, PhysicsComponent>
					updatedPhysics; // Promenljive za ažurirani
									// PhysicsComponent

				for (auto entity : view) {
					auto &physics = view.get<PhysicsComponent>(entity);
					auto &identification = view.get<Identification>(entity);
					auto &sprite = view.get<SpriteComponent>(entity);

					// Ažuriraj grupu ako je tilemap
					if (identification.group == "tilemap") {
						if (identificationGroup != "") {
							identification.group = identificationGroup;
						}
					}

					// Ako je entitet deo selektovane grupe
					if (identification.group == selectedGameObject) {
						// Prikazivanje PhysicsComponent samo jednom
						if (!physicsComponentRendered) {
							renderPhysicsComponent(
								tilemapCursorPos,
								physics); // Render prvi PhysicsComponent
							renderIdentificationComponent(
								tilemapCursorPos, identification, sprite.layer);
							physicsComponentRendered = true;

							updatedPhysics[identification.group] = physics;
							for (auto e : view) {
								auto &oldId = view.get<Identification>(e);
								auto &oldPhysics =
									view.get<PhysicsComponent>(e);

								// Ako je entitet deo selektovane grupe
								if (oldId.group == selectedGameObject &&
									e != entity) {
									auto &physicsWorld = m_Registry->GetContext<
										std::shared_ptr<b2World>>();

									if (!physicsWorld) {
										return;
									}
									m_Registry->GetRegistry()
										.replace<PhysicsComponent>(
											e,
											PhysicsComponent(
												physicsWorld,
												PhysicsAttributes{
													.type =
														physics.GetAttributes()
															.type,
													.density =
														physics.GetAttributes()
															.density,
													.friction =
														physics.GetAttributes()
															.friction,
													.restitution =
														physics.GetAttributes()
															.restitution,
													.gravityScale =
														physics.GetAttributes()
															.gravityScale,
													.position =
														oldPhysics
															.GetAttributes()
															.position,
													.scale =
														oldPhysics
															.GetAttributes()
															.scale,
													.boxSize =
														oldPhysics
															.GetAttributes()
															.boxSize,
													.offset =
														oldPhysics
															.GetAttributes()
															.offset,
													.isSensor =
														physics.GetAttributes()
															.isSensor,
													.isFixedRotation =
														physics.GetAttributes()
															.isFixedRotation}));

									// Ako je entitet već ažuriran
									// oldPhysics.SetAttributes(PhysicsAttributes{
									// 	.type =
									// physics.GetAttributes().type,
									// 	.density =
									// 		physics.GetAttributes().density,
									// 	.friction =
									// 		physics.GetAttributes().friction,
									// 	.restitution =
									// 		physics.GetAttributes().restitution,
									// 	.gravityScale =
									// physics.GetAttributes()
									// 						.gravityScale,
									// 	.position =
									// 		oldPhysics.GetAttributes().position,
									// 	.scale =
									// 		oldPhysics.GetAttributes().scale,
									// 	.boxSize =
									// 		oldPhysics.GetAttributes().boxSize,
									// 	.offset =
									// 		oldPhysics.GetAttributes().offset,
									// 	.isSensor =
									// 		physics.GetAttributes().isSensor,
									// 	.isFixedRotation =
									// 		physics.GetAttributes()
									// 			.isFixedRotation});
								}
							}
						}
					}
				}

				if (!CameraController::paused) {
					ImGui::EndDisabled();
				}
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}

	else {
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
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove;

	if (CameraController::paused) {
		flags |= ImGuiWindowFlags_NoScrollWithMouse;
	}

	ImGui::SetNextWindowSize(ImVec2(widthSize - 230 - 310, heightSize - 300));
	ImGui::SetNextWindowPos(ImVec2(270, 19));
	ImGui::Begin("Camera", nullptr,
				 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

	RenderSceneTabs(*this);
	RenderSceneControls(showCodeEditor, luaScriptContent, luaScriptBuffer,
						newProject);
	RenderActiveScene(m_ActiveScene, flags);

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
	const char *items[] = {"TEXTURES", "FONTS", "MUSIC", "SOUNDFX"};
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
											  &pair.second->volume, 0, 100,
											  "%d")) {
							if (soundPlayer->GetCurrentPlaying()) {
								if (pair.first ==
									soundPlayer->GetCurrentPlaying()
										->GetProperties()
										.name) {
									soundPlayer->MusicVolume(
										pair.first, pair.second->volume);
								}
							}
							// soundPlayer->MusicVolume(soundPlayer->musicVolume);
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
		const char *filters[] = {
			"Image Files\0*.png;*.jpg;*.jpeg\0\0", // TEXTURES
			"Font Files\0*.ttf;*.otf\0\0",		   // FONTS
			"Audio Files\0*.wav\0\0",			   // MUSIC
			"Audio Files\0*.wav\0\0",			   // SOUNDFX
			"Scene Files\0*.scene\0\0"			   // SCENES
		};

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
				for (auto &pair : assetManager->getAllTextures()) {
					ImGui::BeginGroup();
					ImGui::Image(
						(ImTextureID)(intptr_t)pair.second->GetRendererID(),
						ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
					ImGui::Text(pair.first.c_str());
					ImGui::EndGroup();
					ImGui::SameLine(0, 20);
				}
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

			if (ImGui::Button("Import Asset")) {
				std::string filepath =
					Blazr::FileDialog::OpenFile(filters[current_item]);
				if (!filepath.empty()) {

					std::string projectDir =
						Project::GetProjectDirectory().string();
					std::string assetsDir = projectDir + "/assets";

					std::string subfolder;
					switch (current_item) {
					case 0:
						subfolder = "/textures";
						break;
					case 1:
						subfolder = "/fonts";
						break;
					case 2:
						subfolder = "/music";
						break;
					case 3:
						subfolder = "/soundfx";
						break;
					case 4:
						subfolder = "/scenes";
						break;
					default:
						subfolder = "/others";
						break;
					}

					std::string targetDir = assetsDir + subfolder;

					if (!fs::exists(targetDir)) {
						fs::create_directories(targetDir);
					}

					std::string filename =
						fs::path(filepath).filename().string();

					std::string relativePath =
						"assets" + subfolder + "/" + filename;

					std::string targetPath = projectDir + "/" + relativePath;

					try {
						fs::copy(filepath, targetPath,
								 fs::copy_options::overwrite_existing);

						auto &assetManager = Blazr::AssetManager::GetInstance();
						bool success = false;

						switch (current_item) {
						case 0: // TEXTURES
							success = assetManager->LoadTexture(filename,
																relativePath);
							break;
						case 2: // MUSIC
							BLZR_CORE_ERROR("{0}, {1}", filename, relativePath);
							success = assetManager->LoadMusic(
								filename, relativePath, "Imported Music");
							break;
						case 3: // SOUNDFX
							BLZR_CORE_ERROR("SOUND EFFECT");
							success = assetManager->LoadEffect(
								filename, relativePath,
								"Imported Sound Effect");
							break;
						case 4: // SCENES
							success = assetManager->LoadScene(relativePath,
															  m_LuaState);
							break;
						default:
							BLZR_CORE_WARN("Asset type not supported for "
										   "automatic loading.");
							break;
						}

						if (success) {
							ImGui::Text("Imported and Loaded Asset: %s",
										filename.c_str());
						} else {
							ImGui::Text("Failed to load asset: %s",
										filename.c_str());
						}
					} catch (const fs::filesystem_error &e) {
						ImGui::Text("Error copying file: %s", e.what());
					}
				} else {
					ImGui::Text("No file selected.");
				}
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
		if (ImGui::BeginTabItem("Tileset")) {

			auto tilesets = assetManager->GetKeysTexturesTileset();
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
	if (transform.scale.x < 0.01f) {
		transform.scale.x = 0.01f;
	}
	if (transform.scale.x > 10.f) {
		transform.scale.x = 10.f;
	}

	ImGui::SameLine();
	ImGui::Text("Y");
	ImGui::SameLine();
	ImGui::InputFloat("##ScaleY", &transform.scale.y, 0.2f, 1.f, "%.1f");
	if (transform.scale.y < 0.01f) {
		transform.scale.y = 0.01f;
	}
	if (transform.scale.y > 10.f) {
		transform.scale.y = 10.f;
	}

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

	if ((posX != transform.position.x || posY != transform.position.y ||
		 scaleY != transform.scale.x || scaleX != transform.scale.y) &&
		glfwGetKey(m_Window->GetWindow(), GLFW_KEY_ENTER) == GLFW_PRESS) {
		posX = transform.position.x;
		posY = transform.position.y;
		scaleY = transform.scale.y;
		scaleX = transform.scale.x;
		// newPosX = posX;
		// newPosY = posY;
		// newScaleX = scaleX;
		// newScaleY = scaleY;
	}
}

void Editor::renderIdentificationComponent(ImVec2 &cursorPos,
										   Identification &identification,
										   const std::string &layerName) {
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

	char groupBuffer[256];
	strncpy(groupBuffer, identification.group.c_str(), sizeof(groupBuffer) - 1);
	groupBuffer[sizeof(groupBuffer) - 1] = '\0';

	if (ImGui::InputText("###groupObject", groupBuffer, sizeof(groupBuffer),
						 ImGuiInputTextFlags_EnterReturnsTrue)) {
		identification.group = groupBuffer;
	}
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

	static int selectedTextureIndex = -1;
	// Iteriraj kroz mapu i dodaj sve ključeve u vektor
	int i = 0;
	for (const auto &pair : loadedTexture) {
		if (pair.first == "default")
			continue;
		textures.push_back(pair.first);
		if (!sprite.texturePath.empty()) {
			if (sprite.texturePath == pair.first) {
				selectedTextureIndex = i;
			}
		}
		i++;
	}

	// Proveri da li ima tekstura u vektoru
	if (!textures.empty()) {
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

		// Odredi ime koje će se prikazati u Combo kada ništa nije
		// selektovano
		const char *currentTexture =
			selectedTextureIndex == -1 ? "Choose a texture"
									   : textures[selectedTextureIndex].c_str();

		if (selectedTextureIndex == -1) {
			sprite.texturePath = "default";
		}
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

	cursorPos.x -= 64;
	cursorPos.y += 28;
	ImGui::SetCursorPos(cursorPos);
	ImGui::Text("CoordX");
	cursorPos.x += 65;
	cursorPos.y -= 3;
	ImGui::SetCursorPos(cursorPos);
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	ImGui::InputInt("##coordX", &sprite.startX);
	cursorPos.x -= 65;
	cursorPos.y += 28;
	ImGui::SetCursorPos(cursorPos);
	ImGui::Text("CoordY");
	cursorPos.x += 65;
	cursorPos.y -= 3;
	ImGui::SetCursorPos(cursorPos);
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	ImGui::InputInt("##coordY", &sprite.startY);

	if ((sprite.width != spriteWidth || sprite.height != spriteHeight ||
		 sprite.startX != startX || sprite.startY != startY) &&
		glfwGetKey(m_Window->GetWindow(), GLFW_KEY_ENTER) == GLFW_PRESS) {
		auto texture =
			AssetManager::GetInstance()->GetTexture(sprite.texturePath);
		spriteWidth = sprite.width;
		spriteHeight = sprite.height;
		startX = sprite.startX;
		startY = sprite.startY;
		sprite.generateObject(texture->GetWidth(), texture->GetHeight());
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
		std::string absPath = std::filesystem::absolute(path).string();

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
		std::string absPath = std::filesystem::absolute(path).string();

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
	ImGui::InputInt("##widthBoxCollider", &boxCollider.width);
	ImGui::Text("Height");
	ImGui::SameLine(ImGui::GetContentRegionAvail().x - 165);
	ImGui::InputInt("##heightBoxCollider", &boxCollider.height);

	if (boxCollider.width < 16) {
		boxCollider.width = 16;
	}
	if (boxCollider.height < 16) {
		boxCollider.height = 16;
	}

	// boxCollider.width = widthBoxCollider;
	// boxCollider.height = heightBoxCollider;

	ImGui::Text("OffsetX");
	ImGui::SameLine(ImGui::GetContentRegionAvail().x - 165);
	ImGui::InputFloat("##offsetX", &boxCollider.offset.x, 0.1f, 1.0f, "%.1f");
	ImGui::Text("OffsetY");
	ImGui::SameLine(ImGui::GetContentRegionAvail().x - 165);
	ImGui::InputFloat("##offsetY", &boxCollider.offset.y, 0.1f, 1.0f, "%.1f");

	// boxCollider.offset = {offsetX, offsetY};

	ImGui::Text("isColliding");
	ImGui::SameLine(ImGui::GetContentRegionAvail().x - 10);
	ImGui::Checkbox("##colliding", &boxCollider.colliding);

	cursorPos.y += 120;

	if ((widthBoxCollider != boxCollider.width ||
		 heightBoxCollider != boxCollider.height ||
		 offsetX != boxCollider.offset.x || offsetY != boxCollider.offset.y) &&
		glfwGetKey(m_Window->GetWindow(), GLFW_KEY_ENTER) == GLFW_PRESS) {
		widthBoxCollider = boxCollider.width;
		heightBoxCollider = boxCollider.height;
		offsetX = boxCollider.offset.x;
		offsetY = boxCollider.offset.y;
	}
}

void Editor::renderPhysicsComponent(ImVec2 &cursorPos,
									PhysicsComponent &physics) {

	auto body = physics.GetRigidBody();
	if (!body || !body->GetWorld()) {
		BLZR_CORE_ERROR(
			"Invalid body or world while rendering physics component.");
		return;
	}
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
				selectedTypeIndex = i;
				physics.GetAttributes().type = static_cast<RigidBodyType>(i);
				if (CameraController::paused) {
					body->SetType(b2BodyType(i));
				} else {
					BLZR_CORE_WARN(
						"Cannot change body type while simulation is running.");
				}
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
	if (CameraController::paused && body) {
		for (b2Fixture *fixture = body->GetFixtureList(); fixture != nullptr;
			 fixture = fixture->GetNext()) {
			fixture->SetDensity(physics.GetAttributes().density);
			fixture->SetRestitution(physics.GetAttributes().restitution);
			fixture->SetFriction(physics.GetAttributes().friction);
			fixture->SetSensor(physics.GetAttributes().isSensor);
		}
	}
	// } else {
	// 	BLZR_CORE_WARN("Cannot update fixtures while simulation is running.");
	// }

	if ((posX != physicsPosX || posY != physicsPosY || scaleX != newScaleX ||
		 scaleY != newScaleY || widthBoxCollider != newWidthBoxCollider ||
		 heightBoxCollider != newHeightBoxCollider || offsetX != newOffsetX ||
		 offsetY != newOffsetY) &&
		glfwGetKey(m_Window->GetWindow(), GLFW_KEY_ENTER) == GLFW_PRESS) {

		physics.GetAttributes().position = {posX, posY};
		// physics.GetRigidBody()->SetTransform(
		// 	b2Vec2{static_cast<float>(posX), static_cast<float>(posY)}, 0);
		physics.GetAttributes().scale = {scaleX, scaleY};
		physics.GetAttributes().boxSize = {widthBoxCollider, heightBoxCollider};
		physics.GetAttributes().offset = {offsetX, offsetY};
		if (CameraController::paused) {
			physics.init(1280, 720);
		}

		physicsPosX = posX;
		physicsPosY = posY;

		newScaleX = scaleX;
		newScaleY = scaleY;
		newWidthBoxCollider = widthBoxCollider;
		newHeightBoxCollider = heightBoxCollider;
		newOffsetX = offsetX;
		newOffsetY = offsetY;
	}
}
void Editor::renderTileMapSettings(ImVec2 &cursorPos, TilemapScene &tilemap) {
	Ref<Canvas> canvas = tilemap.GetCanvas();

	if (canvas == nullptr) {
		return;
	}

	ImGui::SetCursorPos(cursorPos);
	ImGui::Separator();
	ImGui::Text("TileMap Settings");
	ImGui::SameLine();
	cursorPos.y += 28;
	// Pozicija (Position)
	ImGui::SetCursorPos(cursorPos);
	ImGui::Text("Grid Size");
	cursorPos.y += 18;
	ImGui::SetCursorPos(cursorPos);
	ImGui::PushItemWidth(105);
	ImGui::Text("X");
	ImGui::SameLine();
	ImGui::InputInt("##Grid Size X", &canvasWidth, 1, 4);
	if (canvasWidth < 1) {
		canvasWidth = 1;
	}
	if (canvasWidth > 100) {
		canvasWidth = 100;
	}
	ImGui::SameLine();
	ImGui::Text("Y");
	ImGui::SameLine();
	ImGui::InputInt("##Grid Size Y", &canvasHeight, 1, 4);
	if (canvasHeight < 1) {
		canvasHeight = 1;
	}

	if (canvasHeight > 100) {
		canvasHeight = 100;
	}

	if ((canvasWidth != canvas->GetWidth() ||
		 canvasHeight != canvas->GetHeight()) &&
		glfwGetKey(m_Window->GetWindow(), GLFW_KEY_ENTER) == GLFW_PRESS) {
		canvas->SetWidth(canvasWidth * canvas->GetTileSize());
		canvas->SetHeight(canvasHeight * canvas->GetTileSize());
		canvas->SetUpdate(true);
	}

	ImGui::PopItemWidth();
	cursorPos.y += 35;
	ImGui::SetCursorPosY(cursorPos.y);

	std::vector<Ref<Layer>> layers =
		m_ActiveScene->GetLayerManager()->GetAllLayers();

	layers.erase(std::remove_if(layers.begin(), layers.end(),
								[](const Ref<Layer> &layer) {
									return layer->name == "Grid";
								}),
				 layers.end());

	ImGui::PushItemWidth(120);
	if (ImGui::BeginCombo("##LayerDropdown",
						  selectedTilemapLayer == -1
							  ? "Select a layer:"
							  : layers[selectedTilemapLayer]->name.c_str())) {

		if (ImGui::Selectable("Add New Layer", false)) {
			showAddLayerPopup = true;
			strncpy(newLayerName, "New Layer", sizeof(newLayerName));
			newLayerZIndex = 0;
		}

		for (int i = 0; i < layers.size(); i++) {
			bool isSelected = (selectedTilemapLayer == i);
			if (ImGui::Selectable(layers[i]->name.c_str(), isSelected)) {
				if (layers[i]->name == "Collider") {
					auto tilemapScene =
						std::dynamic_pointer_cast<TilemapScene>(m_ActiveScene);
					if (tilemapScene) {
						tilemapScene->SetSelectedTile(
							std::pair<std::string, glm::vec3>(
								"collider", glm::vec3(0, 0, 0)));
					}
				}
				tilemap.SetLayer(layers[i]->name);
				selectedTilemapLayer = i;
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	ImGui::SameLine();

	if (tilemap.GetLayer() == "Collider") {
		char groupBuffer[256];
		strncpy(groupBuffer, identificationGroup.c_str(),
				sizeof(groupBuffer) - 1);
		groupBuffer[sizeof(groupBuffer) - 1] = '\0';

		if (ImGui::InputText("###group", groupBuffer, sizeof(groupBuffer),
							 ImGuiInputTextFlags_EnterReturnsTrue)) {
			identificationGroup = groupBuffer;
			if (identificationGroup.empty()) {
				identificationGroup = "default";
			}
		}
	}
	ImGui::PopItemWidth();

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
			selectedTilemapLayer = layers.size() - 1;

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

	cursorPos.y += 25;

	// ImGui::SetCursorPos(cursorPos);
	// ImGui::Text("Scale");
	// cursorPos.y += 18;
	//
	// ImGui::SetCursorPos(cursorPos);
	// ImGui::PushItemWidth(105);
	// ImGui::Text("X");
	// ImGui::SameLine();
	// ImGui::InputInt("##ScaleX", &tilemapScaleX, 1, 4);
	// if (tilemapScaleX < 1) {
	// 	tilemapScaleX = 1;
	// }
	// if (tilemapScaleX > 10) {
	// 	tilemapScaleX = 10;
	// }
	// ImGui::SameLine();
	// ImGui::Text("Y");
	// ImGui::SameLine();
	// ImGui::InputInt("##ScaleY", &tilemapScaleY, 1, 4);
	// if (tilemapScaleY < 1) {
	// 	tilemapScaleY = 1;
	// }
	//
	// if (tilemapScaleY > 100) {
	// 	tilemapScaleY = 100;
	// }
	//
	// if ((tilemapScaleX != canvas->GetTileSize() ||
	// 	 tilemapScaleY != tilemap.GetScale().y) &&
	// 	glfwGetKey(m_Window->GetWindow(), GLFW_KEY_ENTER) == GLFW_PRESS) {
	// 	tilemap.SetScale(glm::vec2(tilemapScaleX, tilemapScaleY));
	// }
	//
	// ImGui::PopItemWidth();
}

} // namespace Blazr
