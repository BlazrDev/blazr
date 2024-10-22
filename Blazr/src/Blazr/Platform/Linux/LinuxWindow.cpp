#include "blzrpch.h"
#include "Blazr/Core/Core.h"
#include "Blazr/Core/Log.h"
#include "Blazr/Ecs/Entity.h"
#include "Blazr/Ecs/Registry.h"
#include "Blazr/Events/ApplicationEvent.h"
#include "Blazr/Events/KeyEvent.h"
#include "Blazr/Events/MouseEvent.h"
#include "Blazr/Renderer/RendererAPI.h"
#include "Blazr/Renderer/ShaderLoader.h"
#include "Blazr/Renderer/Texture2D.h"
#include "LinuxWindow.h"

namespace Blazr {

Camera2D camera = Camera2D(1280, 720);
std::unique_ptr<Registry> registry = std::make_unique<Registry>();

static bool s_GLFWInitialized = false;
bool mousePressed = false;
double lastMouseX, lastMouseY;

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
	// Renderer2D::s_Registry = std::make_unique<Blazr::Registry>();

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

	glfwSetMouseButtonCallback(
		m_Window, [](GLFWwindow *window, int button, int action, int mods) {
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
			double xpos, ypos;

			glfwGetCursorPos(window, &xpos, &ypos);
			switch (action) {
			case GLFW_PRESS: {
				MouseButtonPressedEvent event(button);
				data.eventCallback(event);
				break;
			}
			case GLFW_RELEASE: {
				MouseButtonReleasedEvent event(button);
				data.eventCallback(event);
				break;
			}
			}
		});

	// TODO: napraviti da na scroll prati kursor
	glfwSetScrollCallback(m_Window, [](GLFWwindow *window, double xOffset,
									   double yOffset) {
		WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
		MouseScrolledEvent event((float)xOffset, (float)yOffset);
		data.eventCallback(event);
		// Camera2D &camera = data.m_Renderer->GetCamera();
		// Camera2D &camera = ca;
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
		camera.SetPosition(
			mousePosNormalized *
			camera.GetScale()); // Pomeranje kamere prema poziciji miša
	});

	// glfwSetCursorPosCallback(
	// 	m_Window, [](GLFWwindow *window, double xPos, double yPos) {
	// 		WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
	// 		MouseMovedEvent event((float)xPos, (float)yPos);
	// 		data.eventCallback(event);
	// 	});

	glfwSetMouseButtonCallback(
		m_Window, [](GLFWwindow *window, int button, int action,
					 int mods) { // Uhvatite varijable prema referenci
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

			if (button == GLFW_MOUSE_BUTTON_LEFT) {
				if (action == GLFW_PRESS) {
					mousePressed = true;
					glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
				} else if (action == GLFW_RELEASE) {
					mousePressed = false;
				}
			}
		});

	glfwSetCursorPosCallback(
		m_Window, [](GLFWwindow *window, double xpos,
					 double ypos) { // Uhvatite varijable prema referenci
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

			if (mousePressed) {
				// Camera2D &camera = data.m_Renderer->GetCamera();
				double deltaX = xpos - lastMouseX;
				double deltaY = ypos - lastMouseY;

				// glm::vec2 cameraPos = camera.GetPosition();
				// cameraPos.x -= deltaX * camera.GetScale() * 0.1f;
				// cameraPos.y += deltaY * camera.GetScale() * 0.1f;
				//
				// camera.SetPosition(cameraPos);

				lastMouseX = xpos;
				lastMouseY = ypos;

				auto view = registry->GetRegistry()
								.view<TransformComponent, SpriteComponent>();
				for (auto entity : view) {
					auto &transform = view.get<TransformComponent>(entity);
					auto &sprite = view.get<SpriteComponent>(entity);
					// transform.position.x -= deltaX * camera.GetScale() *
					// 0.1f; transform.position.y += deltaY * camera.GetScale()
					// * 0.1f;
				}
				camera.SetPosition(
					{camera.GetPosition().x - deltaX * camera.GetScale() * 0.3f,
					 camera.GetPosition().y +
						 deltaY * camera.GetScale() * 0.3f});
			}
		});
	// m_Data.m_Renderer->GetCamera().SetScale(1.f);
	// loadTexture("assets/chammy.png");
	// m_Data.m_Renderer->EndBatch();
	// m_Data.m_Renderer->Flush();

	// Camera2D &camera = m_Data.m_Renderer->GetCamera();
	// auto projection = camera.GetCameraMatrix();
	// GLuint location = glGetUniformLocation(
	// 	m_Data.m_Renderer->GetShaderProgramID(), "uProjection");
	//
	// glUniformMatrix4fv(location, 1, GL_FALSE, &projection[0][0]);
	//
	// m_Data.m_Renderer->BeginBatch();
	//
	// Entity entity2 = Entity(*m_Data.s_Registry, "Ent1", "G1");
	//
	// auto &transform2 = entity2.AddComponent<TransformComponent>(
	// 	TransformComponent{.position = glm::vec2(10.0f, 30.0f),
	// 					   .scale = glm::vec2(1.0f, 1.0f),
	// 					   .rotation = 0.0f});
	//
	// auto &sprite2 = entity2.AddComponent<SpriteComponent>(SpriteComponent{
	// 	.width = 1.0f, .height = 2.0f, .startX = 10, .startY = 30});
	//
	// m_Data.m_Renderer->DrawRectangle(transform2.position.x - sprite2.width /
	// 2, 								 transform2.position.y - sprite2.height
	// / 2, 								 sprite2.width, sprite2.height,
	// {1.0f, 1.0f, 0.0f, 1.0f});
	//
	// Entity entity = Entity(*m_Data.s_Registry, "Ent1", "G1");
	//
	// auto &transform = entity.AddComponent<TransformComponent>(
	// 	TransformComponent{.position = glm::vec2(10.0f, 50.0f),
	// 					   .scale = glm::vec2(1.0f, 1.0f),
	// 					   .rotation = 0.0f});
	//
	// auto &sprite = entity.AddComponent<SpriteComponent>(SpriteComponent{
	// 	.width = 20.0f, .height = 5.0f, .startX = 10, .startY = 30});
	//
	// m_Data.m_Renderer->DrawRectangle(transform2.position.x - sprite2.width /
	// 2, 								 transform2.position.y - sprite2.height
	// / 2, 								 sprite2.width, sprite2.height,
	// {1.0f, 0.0f, 0.f, 1.0f});
	//
	// m_Data.m_Renderer->EndBatch();
	// m_Data.m_Renderer->Flush();
	// camera.Update();
	// m_Data.m_Renderer->PollEvents();
	// m_Data.m_Renderer->SwapBuffers();
	// m_Data.m_Renderer->Clear();
	camera.SetScale(1.0f);
	camera.SetPosition({0.0f, 0.0f});
	glm::vec2 pos = {0.f, 0.f};
	glm::vec2 size = {200.f, 200.f};
	glm::vec4 color = {1.f, 0.f, 0.f, 1.f};

	Entity entity = Entity(*registry, "Ent1", "G1");
	auto &transform =
		entity.AddComponent<TransformComponent>(TransformComponent{
			.position = pos, .scale = glm::vec2(1.0f, 1.0f), .rotation = 0.0f});

	auto &sprite = entity.AddComponent<SpriteComponent>(SpriteComponent{
		.width = size[0], .height = size[1], .startX = 10, .startY = 30});

	Renderer2D::BeginScene(camera);
	Renderer2D::DrawQuad(entity.GetEntityHandler(), pos, size,
						 Texture2D::Create("assets/chammy.png"));
	Renderer2D::Flush();
}

void LinuxWindow::shutdown() { Renderer2D::Shutdown(); }

void LinuxWindow::onUpdate() {
	glfwPollEvents();
	Renderer2D::BeginScene(camera);
	auto view =
		registry->GetRegistry().view<TransformComponent, SpriteComponent>();
	for (auto entity : view) {
		auto &transform = view.get<TransformComponent>(entity);
		auto &sprite = view.get<SpriteComponent>(entity);
		// Renderer2D::DrawQuad(entity);
		Renderer2D::DrawQuad(entity, transform.position,
							 {sprite.width, sprite.height},
							 Texture2D::Create("assets/chammy.png"));
	}
	// Renderer2D::DrawQuad(0, {0.f, 0.f}, {200.f, 200.f},
	// 					 Texture2D::Create("assets/chammy.png"));
	Renderer2D::Flush();

	camera.Update();
	glfwSwapBuffers(m_Window);
	Renderer2D::Clear();

	// void LinuxWindow::loadTexture(const std::string &path) {
	// 	float vertices[] = {
	// 		// Positions          // Texture Coords
	// 		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // Bottom-left
	// 		0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, // Bottom-right
	// 		0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // Top-right
	// 		-0.5f, 0.5f,  0.0f, 0.0f, 1.0f	// Top-left
	// 	};
	// 	unsigned int indices[] = {
	// 		0, 1, 2, // First triangle
	// 		2, 3, 0	 // Second triangle
	// 	};
	// 	// Create buffers and array objects
	// 	m_Shader =
	// 		Blazr::ShaderLoader::Create("shaders/vertex/TextureTestShader.vert",
	// 									"shaders/fragment/TextureTestShader.frag");
	//
	// 	if (!m_Shader) {
	// 		BLZR_CORE_ERROR("Failed to load shader");
	// 	}
	//
	// 	m_Shader->Enable();
	// 	GLuint VAO, VBO, EBO;
	// 	glGenVertexArrays(1, &VAO);
	// 	glGenBuffers(1, &VBO);
	// 	glGenBuffers(1, &EBO);
	// 	// Bind VAO and VBO/EBO
	// 	glBindVertexArray(VAO);
	// 	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// 	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
	// GL_STATIC_DRAW); 	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// 	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
	// 				 GL_STATIC_DRAW);
	// 	// Set vertex attributes
	// 	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
	// 						  (void *)0);
	// 	glEnableVertexAttribArray(0);
	// 	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
	// 						  (void *)(3 * sizeof(float)));
	// 	glEnableVertexAttribArray(1);
	// 	glBindVertexArray(0);
	// 	Texture2D texture = Texture2D(path);
	// 	//
	// glUniform1i(glGetUniformLocation(m_Data.m_Renderer->GetShaderProgramID(),
	// 	// 								 "texture"),
	// 	// 			0);
	// 	glActiveTexture(GL_TEXTURE0);
	// 	glBindTexture(GL_TEXTURE_2D, texture.GetID());
	// 	glBindVertexArray(VAO);
	//
	// 	// m_Data.m_Renderer->GetCamera().SetScale(5.f);
	// 	Camera2D &camera = m_Data.m_Renderer->GetCamera();
	// 	auto projection = camera.GetCameraMatrix();
	// 	GLuint location = glGetUniformLocation(
	// 		m_Data.m_Renderer->GetShaderProgramID(), "uProjection");
	// 	//
	// 	glUniformMatrix4fv(location, 1, GL_FALSE, &projection[0][0]);
	//
	// 	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	// 	m_Shader->Disable();
	// 	m_Data.m_Renderer->BeginBatch();
	//
	// 	Entity entity2 = Entity(*m_Data.s_Registry, "Ent1", "G1");
	//
	// 	auto &transform2 = entity2.AddComponent<TransformComponent>(
	// 		TransformComponent{.position = glm::vec2(10.0f, 30.0f),
	// 						   .scale = glm::vec2(1.0f, 1.0f),
	// 						   .rotation = 0.0f});
	//
	// 	auto &sprite2 = entity2.AddComponent<SpriteComponent>(SpriteComponent{
	// 		.width = 1.0f, .height = 2.0f, .startX = 10, .startY = 30});
	//
	// 	m_Data.m_Renderer->DrawRectangle(transform2.position.x - sprite2.width /
	// 2, 									 transform2.position.y -
	// sprite2.height / 2, 									 sprite2.width,
	// sprite2.height, 									 {1.0f, 1.0f,
	// 0.0f, 1.0f});
	//
	// 	Entity entity = Entity(*m_Data.s_Registry, "Ent1", "G1");
	//
	// 	auto &transform = entity.AddComponent<TransformComponent>(
	// 		TransformComponent{.position = glm::vec2(10.0f, 50.0f),
	// 						   .scale = glm::vec2(1.0f, 1.0f),
	// 						   .rotation = 0.0f});
	//
	// 	auto &sprite = entity.AddComponent<SpriteComponent>(SpriteComponent{
	// 		.width = 20.0f, .height = 5.0f, .startX = 10, .startY = 30});
	//
	// 	m_Data.m_Renderer->DrawRectangle(transform2.position.x - sprite2.width /
	// 2, 									 transform2.position.y -
	// sprite2.height / 2, 									 sprite2.width,
	// sprite2.height, 									 {1.0f,
	// 0.0f, 1.f, 1.0f});
	//
	// 	m_Data.m_Renderer->EndBatch();
	// 	m_Data.m_Renderer->Flush();
	// 	camera.Update();
	// 	m_Data.m_Renderer->PollEvents();
	// 	m_Data.m_Renderer->SwapBuffers();
	// 	m_Data.m_Renderer->Clear();
	// }
}
void LinuxWindow::setVSync(bool enabled) {

	if (enabled)
		glfwSwapInterval(1);
	else
		glfwSwapInterval(0);
	m_Data.vsync = enabled;
}

bool LinuxWindow::isVSync() const { return m_Data.vsync; }

void LinuxWindow::setEventCallback(
	const LinuxWindow::EventCallbackFn &callback) {
	m_Data.eventCallback = callback;
}

} // namespace Blazr
