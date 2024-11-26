#include "blzrpch.h"
#include "Blazr/Core/Core.h"
#include "Blazr/Core/Log.h"
#include "Blazr/Ecs/Components/BoxColliderComponent.h"
#include "Blazr/Ecs/Components/PhysicsComponent.h"
#include "Blazr/Ecs/Components/SpriteComponent.h"
#include "Blazr/Ecs/Components/TransformComponent.h"
#include "Blazr/Ecs/Entity.h"
#include "Blazr/Ecs/Registry.h"
#include "Blazr/Events/ApplicationEvent.h"
#include "Blazr/Events/KeyEvent.h"
#include "Blazr/Events/MouseEvent.h"
#include "Blazr/Physics/Box2DWrapper.h"
#include "Blazr/Renderer/Renderer2D.h"
#include "Blazr/Renderer/ShaderLoader.h"
#include "Blazr/Renderer/Texture2D.h"
#include "Blazr/Systems/AnimationSystem.h"
#include "Blazr/Systems/PhysicsSystem.h"
#include "Blazr/Systems/ScriptingSystem.h"
#include "Blazr/Systems/Sounds/SoundPlayer.h"
#include "Blazr/Systems/Sounds/SoundProperties.h"
#include "LinuxWindow.h"
#include "ext/vector_float4.hpp"
#include <Blazr/Resources/AssetManager.h>
#include <memory>

namespace Blazr {
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

	glfwSetFramebufferSizeCallback(
		m_Window, [](GLFWwindow *window, int width, int height) {
			glViewport(0, 0, width, height);
		});

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
}

void LinuxWindow::shutdown() { Renderer2D::Shutdown(); }

void LinuxWindow::onUpdate() {
	glfwPollEvents();
	glfwSwapBuffers(m_Window);
}

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

GLFWwindow BLZR_API *Blazr::LinuxWindow::GetWindow() const { return m_Window; }
} // namespace Blazr
