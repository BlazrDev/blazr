#include "blzrpch.h"
#include "Blazr/Core/Core.h"
#include "Blazr/Core/Log.h"
#include "Blazr/Ecs/Entity.h"
#include "Blazr/Ecs/Registry.h"
#include "Blazr/Events/ApplicationEvent.h"
#include "Blazr/Events/KeyEvent.h"
#include "Blazr/Events/MouseEvent.h"
#include "Blazr/Renderer/Renderer2D.h"
#include "Blazr/Renderer/ShaderLoader.h"
#include "Blazr/Renderer/Texture2D.h"
#include "Blazr/Systems/ScriptingSystem.h"
#include "LinuxWindow.h"
#include "ext/vector_float4.hpp"

namespace Blazr {

Camera2D camera = Camera2D(1280, 720);
std::unique_ptr<Registry> registry = std::make_unique<Registry>();
entt::entity selectedEntity =
	entt::null;				   // Drži referencu na selektovani entitet
bool isEntitySelected = false; // Da li je entitet selektovan
bool mousePressed = false;	   // Da li je dugme miša pritisnuto
double lastMouseX, lastMouseY; // Poslednja pozicija miša

static bool s_GLFWInitialized = false;

static void GLFWErorCallback(int error, const char *description) {
	BLZR_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
}

Window *Window::create(const WindowProperties &properties) {
	return new LinuxWindow(properties);
}

LinuxWindow::LinuxWindow(const WindowProperties &properties) {
	init(properties);
}

LinuxWindow::~LinuxWindow() { shutdown(); }

unsigned int LinuxWindow::getHeight() const { return m_Data.height; }
unsigned int LinuxWindow::getWidth() const { return m_Data.width; }

void LinuxWindow::init(const WindowProperties &properties) {

	if (!glfwInit()) {
		BLZR_CORE_ERROR("Failed to initialize GLFW!");
		return;
	}

	m_Window = glfwCreateWindow(1280, 720, "OpenGL Renderer", nullptr, nullptr);
	if (!m_Window) {
		BLZR_CORE_ERROR("Failed to create window!");
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(m_Window);

	if (glewInit() != GLEW_OK) {
		BLZR_CORE_ERROR("Failed to initialize GLEW!");
		return;
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	int width;
	int height;
	glfwGetWindowSize(m_Window, &width, &height);

	m_Data.title = properties.Title;
	m_Data.width = width;
	m_Data.height = height;

	BLZR_CORE_INFO("Creating window {0} ({1}, {2})", properties.Title,
				   properties.Width, properties.Height);

	setVSync(true);
	Renderer2D::Init();

	glfwSetWindowUserPointer(m_Window, &m_Data);

	glfwSetWindowSizeCallback(
		m_Window, [](GLFWwindow *window, int width, int height) {
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
			data.width = width;
			data.height = height;

			WindowResizeEvent event(width, height);
			data.eventCallback(event);
		});

	glfwSetWindowCloseCallback(m_Window, [](GLFWwindow *window) {
		WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
		WindowCloseEvent event;
		data.eventCallback(event);
	});

	glfwSetKeyCallback(m_Window, [](GLFWwindow *window, int key, int scancode,
									int action, int mods) {
		WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

		switch (action) {
		case GLFW_PRESS: {
			KeyPressedEvent event(key, 0);
			data.eventCallback(event);
			break;
		}
		case GLFW_RELEASE: {
			KeyReleasedEvent event(key);
			data.eventCallback(event);
			break;
		}
		case GLFW_REPEAT: {
			KeyPressedEvent event(key, 1);
			data.eventCallback(event);
			break;
		}
		}
	});

	// TODO remove tmp code
	// Creating lua state
	auto lua = std::make_shared<sol::state>();

	if (!lua) {
		BLZR_CORE_ERROR("Failed to create the lua state!");
		return;
	}

	lua->open_libraries(sol::lib::base, sol::lib::math, sol::lib::os,
						sol::lib::table, sol::lib::io, sol::lib::string);

	if (!m_Data.m_Registry->AddToContext<std::shared_ptr<sol::state>>(lua)) {
		BLZR_CORE_ERROR(
			"Failed to add the sol::state to the registry context!");
		return;
	}

	auto scriptSystem = std::make_shared<ScriptingSystem>(*m_Data.m_Registry);
	if (!scriptSystem) {
		BLZR_CORE_ERROR("Failed to create the script system!");
		return;
	}

	if (!scriptSystem->LoadMainScript(*lua)) {
		BLZR_CORE_ERROR("Failed to load the main lua script");
		return;
	}

	if (!m_Data.m_Registry->AddToContext<std::shared_ptr<ScriptingSystem>>(
			scriptSystem)) {
		BLZR_CORE_ERROR(
			"Failed to add the script system to the registry context!");
		return;
	}

	// glfwSetMouseButtonCallback(
	// 	m_Window, [](GLFWwindow *window, int button, int action, int mods) {
	// 		WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
	// 		double xpos, ypos;
	//
	// 		glfwGetCursorPos(window, &xpos, &ypos);
	// 		switch (action) {
	// 		case GLFW_PRESS: {
	// 			MouseButtonPressedEvent event(button);
	// 			data.eventCallback(event);
	// 			break;
	// 		}
	// 		case GLFW_RELEASE: {
	// 			MouseButtonReleasedEvent event(button);
	// 			data.eventCallback(event);
	// 			break;
	// 		}
	// 		}
	// 	});
	glfwSetMouseButtonCallback(m_Window, [](GLFWwindow *window, int button,
											int action, int mods) {
		WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			if (action == GLFW_PRESS) {
				mousePressed = true; // Dugme miša je pritisnuto
				BLZR_CORE_INFO("Mouse button pressed");
				double mouseX, mouseY;
				glfwGetCursorPos(window, &mouseX, &mouseY);

				int screenWidth, screenHeight;
				glfwGetWindowSize(window, &screenWidth, &screenHeight);

				glm::vec2 mouseWorldPos = data.GetWorldCoordinates(
					mouseX, mouseY, camera, screenWidth, screenHeight);

				// Proveri da li je miš iznad nekog entiteta
				auto view = registry->GetRegistry()
								.view<TransformComponent, SpriteComponent>();
				for (auto entity : view) {
					auto &transform = view.get<TransformComponent>(entity);
					auto &sprite = view.get<SpriteComponent>(entity);

					if (data.IsMouseOverEntity(
							mouseWorldPos, transform.position,
							{sprite.width, sprite.height}, transform.scale,
							camera.GetScale())) {
						BLZR_CORE_INFO("Mouse is over entity {0}",
									   sprite.texturePath);
						selectedEntity = entity; // Postavi selektovani entitet
						isEntitySelected =
							true; // Obeleži entitet kao selektovan
						lastMouseX = mouseX;
						lastMouseY = mouseY;
						break;
					}
				}
			} else if (action == GLFW_RELEASE) {
				mousePressed = false;	  // Dugme miša je pušteno
				isEntitySelected = false; // Oslobodi selekciju
			}
		}
	});

	// TODO: napraviti da na scroll prati kursor
	glfwSetScrollCallback(m_Window, [](GLFWwindow *window, double xOffset,
									   double yOffset) {
		WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
		MouseScrolledEvent event((float)xOffset, (float)yOffset);
		data.eventCallback(event);
		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);
		glm::vec2 mousePosNormalized = glm::vec2(
			2.0f * (mouseX / data.width) - 1.0f, // Normalizacija X koordinate
			1.0f - 2.0f * (mouseY / data.height) // Normalizacija Y koordinate
		);

		if (yOffset > 0) {
			camera.SetScale(camera.GetScale() + 0.1f);
		} else {
			camera.SetScale(camera.GetScale() - 0.1f);
		}
		// camera.SetPosition(
		// 	mousePosNormalized *
		// 	camera.GetScale()); // Pomeranje kamere prema poziciji miša
	});

	// glfwSetCursorPosCallback(
	// 	m_Window, [](GLFWwindow *window, double xPos, double yPos) {
	// 		WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
	// 		MouseMovedEvent event((float)xPos, (float)yPos);
	// 		data.eventCallback(event);
	// 	});

	// glfwSetMouseButtonCallback(
	// 	m_Window, [](GLFWwindow *window, int button, int action,
	// 				 int mods) { // Uhvatite varijable prema referenci
	// 		WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
	//
	// 		if (button == GLFW_MOUSE_BUTTON_LEFT) {
	// 			if (action == GLFW_PRESS) {
	// 				mousePressed = true;
	// 				glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
	// 			} else if (action == GLFW_RELEASE) {
	// 				mousePressed = false;
	// 			}
	// 		}
	// 	});

	// glfwSetMouseButtonCallback(m_Window, [](GLFWwindow *window, int button,
	// 										int action, int mods) {
	// 	WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
	// 	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
	// 		BLZR_CORE_INFO("Mouse button pressed");
	// 		double mouseX, mouseY;
	// 		glfwGetCursorPos(window, &mouseX, &mouseY);
	//
	// 		int screenWidth, screenHeight;
	// 		glfwGetWindowSize(window, &screenWidth, &screenHeight);
	//
	// 		glm::vec2 mouseWorldPos = data.GetWorldCoordinates(
	// 			mouseX, mouseY, camera, screenWidth, screenHeight);
	//
	// 		auto view = registry->GetRegistry()
	// 						.view<TransformComponent, SpriteComponent>();
	// 		for (auto entity : view) {
	// 			auto &transform = view.get<TransformComponent>(entity);
	// 			auto &sprite = view.get<SpriteComponent>(entity);
	//
	// 			if (data.IsMouseOverEntity(mouseWorldPos, transform.position,
	// 									   {sprite.width, sprite.height},
	// 									   transform.scale)) {
	// 				BLZR_CORE_INFO("Mouse is over entity {0}",
	// 							   sprite.texturePath);
	// 				// Store selected entity somewhere or add a
	// 				// SelectedComponent to it
	// 				selectedEntity = entity;
	// 			} else {
	// 				selectedEntity = entt::null;
	// 			}
	// 		}
	// 	}
	// });
	//
	// glfwSetCursorPosCallback(
	// 	m_Window, [](GLFWwindow *window, double xpos,
	// 				 double ypos) { // Uhvatite varijable prema referenci
	// 		WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
	//
	// 		if (mousePressed) {
	// 			double deltaX = xpos - lastMouseX;
	// 			double deltaY = ypos - lastMouseY;
	//
	// 			lastMouseX = xpos;
	// 			lastMouseY = ypos;
	// 			camera.SetPosition(camera.GetPosition() +
	// 							   glm::vec2(deltaX, deltaY));
	// 		}
	// 	});
	glfwSetCursorPosCallback(m_Window, [](GLFWwindow *window, double xpos,
										  double ypos) {
		WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
		double deltaX = xpos - lastMouseX;
		double deltaY = ypos - lastMouseY;

		lastMouseX = xpos;
		lastMouseY = ypos;
		if (mousePressed && isEntitySelected && selectedEntity != entt::null) {
			BLZR_CORE_INFO("Mouse position: {0}, {1}", xpos, ypos);
			// Izračunaj pomeraj miša

			// Ažuriraj poslednju poziciju miša

			// Ažuriraj poziciju selektovanog entiteta
			auto &transform =
				registry->GetRegistry().get<TransformComponent>(selectedEntity);
			transform.position.x += deltaX; // Pomeraj u X
			transform.position.y +=
				deltaY; // Pomeraj u Y (zavisno od orijentacije)

			BLZR_CORE_INFO("Entity moved to: {0}, {1}", transform.position.x,
						   transform.position.y);
		} else if (mousePressed && !isEntitySelected) {
			camera.SetPosition(camera.GetPosition() +
							   glm::vec2(deltaX, deltaY) * 0.1f);
		}
	});

	camera.SetScale(1.0f);
	camera.SetPosition({0.0f, 0.0f});
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

	glm::vec2 pos2 = {300.f, 300.f};
	glm::vec2 size2 = {200.f, 200.f};
	glm::vec4 color2 = {0.f, 1.f, 0.f, 1.f};

	/*Entity entity2 = Entity(*registry, "Ent2", "G2");
	auto &transform2 = entity2.AddComponent<TransformComponent>(
		TransformComponent{.position = pos2,
						   .scale = glm::vec2(2.0f, 2.0f),
						   .rotation = 0.0f});

	auto &sprite2 = entity2.AddComponent<SpriteComponent>(
		SpriteComponent{.width = size2[0],
						.height = size2[1],
						.startX = 100,
						.startY = 100,
						.texturePath = "assets/masha.png"});*/
	Renderer2D::BeginScene(camera);

	// Renderer2D::DrawQuad(entity.GetEntityHandler(), pos, size,
	// 					 Texture2D::Create("assets/chammy.png"),
	// 					 transform.rotation);
	/*Renderer2D::DrawQuad(entity2.GetEntityHandler(), pos2, size2,
						 Texture2D::Create("assets/masha.png"), 1.0f, color2);*/

	Renderer2D::Flush();
}

void LinuxWindow::shutdown() { Renderer2D::Shutdown(); }

void LinuxWindow::onUpdate() {

	auto &scriptSystem =
		m_Data.m_Registry->GetContext<std::shared_ptr<ScriptingSystem>>();

	scriptSystem->Update();
	scriptSystem->Render();

	glfwPollEvents();
	camera.Update();

	Renderer2D::BeginScene(camera);
	auto view =
		registry->GetRegistry().view<TransformComponent, SpriteComponent>();
	for (auto entity : view) {
		auto &transform = view.get<TransformComponent>(entity);
		auto &sprite = view.get<SpriteComponent>(entity);
		Renderer2D::DrawQuad(*registry, entity);
	}
	Renderer2D::Flush();

	glfwSwapBuffers(m_Window);
	Renderer2D::Clear();
}

} // namespace Blazr
void Blazr::LinuxWindow::setVSync(bool enabled) {

	if (enabled)
		glfwSwapInterval(1);
	else
		glfwSwapInterval(0);
	m_Data.vsync = enabled;
}

bool Blazr::LinuxWindow::isVSync() const { return m_Data.vsync; }

void Blazr::LinuxWindow::setEventCallback(
	const LinuxWindow::EventCallbackFn &callback) {
	m_Data.eventCallback = callback;
}
