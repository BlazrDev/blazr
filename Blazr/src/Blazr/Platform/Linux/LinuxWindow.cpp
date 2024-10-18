#include "blzrpch.h"
#include "Blazr/Core/Core.h"
#include "Blazr/Core/Log.h"
#include "Blazr/Events/ApplicationEvent.h"
#include "Blazr/Events/KeyEvent.h"
#include "Blazr/Events/MouseEvent.h"
#include "Blazr/Renderer/RendererAPI.h"
#include "Blazr/Renderer/Texture2D.h"
#include "LinuxWindow.h"

namespace Blazr {

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
	m_Data.m_Renderer = new RendererAPI();

	if (!m_Data.m_Renderer->Init()) {
		BLZR_CORE_ERROR("Failed to initialize RendererAPI");
		return;
	}

	m_Window = m_Data.m_Renderer->getWindow();
	int width, height;
	glfwGetWindowSize(m_Window, &width, &height);

	m_Data.title = properties.Title;
	m_Data.width = width;
	m_Data.height = height;
	m_Data.m_Renderer->CreateCamera(800.0f, 600.0f);

	BLZR_CORE_INFO("Creating window {0} ({1}, {2})", properties.Title,
				   properties.Width, properties.Height);

	setVSync(true);

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
				data.createRect(xpos, ypos);
				break;
			}
			case GLFW_RELEASE: {
				MouseButtonReleasedEvent event(button);
				data.eventCallback(event);
				break;
			}
			}
		});

	glfwSetScrollCallback(
		m_Window, [](GLFWwindow *window, double xOffset, double yOffset) {
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.eventCallback(event);
		});

	glfwSetCursorPosCallback(
		m_Window, [](GLFWwindow *window, double xPos, double yPos) {
			WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
			MouseMovedEvent event((float)xPos, (float)yPos);
			data.eventCallback(event);
		});
}

void LinuxWindow::shutdown() { m_Data.m_Renderer->Shutdown(); }

void LinuxWindow::onUpdate() {

	Camera2D &camera = m_Data.m_Renderer->GetCamera();
	camera.SetPosition(glm::vec2(0.0f, 0.0f));
	camera.SetScale(10.0f);
	auto projection = camera.GetCameraMatrix();
	GLuint location = glGetUniformLocation(
		m_Data.m_Renderer->GetShaderProgramID(), "uProjection");

	glUniformMatrix4fv(location, 1, GL_FALSE, &projection[0][0]);

	m_Data.m_Renderer->BeginBatch();

	auto registry = std::make_unique<Blazr::Registry>();
	Entity entity2 = Entity(*registry, "Ent1", "G1");

	auto &transform2 = entity2.AddComponent<TransformComponent>(
		TransformComponent{.position = glm::vec2(10.0f, 30.0f),
						   .scale = glm::vec2(1.0f, 1.0f),
						   .rotation = 0.0f});

	auto &sprite2 = entity2.AddComponent<SpriteComponent>(SpriteComponent{
		.width = 1.0f, .height = 2.0f, .startX = 10, .startY = 30});

	m_Data.m_Renderer->DrawRectangle(transform2.position.x - sprite2.width / 2,
									 transform2.position.y - sprite2.height / 2,
									 sprite2.width, sprite2.height,
									 {1.0f, 1.0f, 0.0f, 1.0f});

	// float vertices[] = {
	// 	40.0f,	40.0f,	0.0f, // Vertex 1 (x, y, z)
	// 	260.0f, 40.0f,	0.0f, // Vertex 2 (x, y, z)
	// 	260.0f, 260.0f, 0.0f, // Vertex 3 (x, y, z)
	// };
	// GLuint VAO, VBO;
	// glGenVertexArrays(1, &VAO);
	// glBindVertexArray(VAO);
	//
	// // 2. Generate and bind the Vertex Buffer Object
	// glGenBuffers(1, &VBO);
	// glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
	// GL_STATIC_DRAW);
	//
	// // 3. Specify vertex attributes (position in this case)
	// // Enable the attribute (location 0 here corresponds to position)
	// glEnableVertexAttribArray(0);
	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
	// 					  (void *)0);
	//
	// // 4. Unbind the VBO and VAO to reset state (optional, but good practice)
	// glBindBuffer(GL_ARRAY_BUFFER, 0);
	// glBindVertexArray(0);
	//
	// // 5. In the render loop, bind the VAO and draw the triangle
	// glBindVertexArray(VAO);
	// glDrawArrays(GL_TRIANGLES, 0, 3); // Draw the triangle
	// glBindVertexArray(0);			  // Unbind the VAO after drawing

	// TODO remove tmp test code
	// float vertices[] = {
	// 	// Positions          // Texture Coords
	// 	40.0f,	40.0f,	0.0f, 0.0f, 0.0f, // Bottom-left
	// 	260.0f, 40.0f,	0.0f, 1.0f, 0.0f, // Bottom-right
	// 	260.0f, 260.0f, 0.0f, 1.0f, 1.0f, // Top-right
	// 	40.0f,	260.0f, 0.0f, 0.0f, 1.0f  // Top-left
	// };
	//
	// unsigned int indices[] = {
	// 	0, 1, 2, // First triangle
	// 	2, 3, 0	 // Second triangle
	// };
	//
	// // Create buffers and array objects
	// GLuint VAO, VBO, EBO;
	// glGenVertexArrays(1, &VAO);
	// glGenBuffers(1, &VBO);
	// glGenBuffers(1, &EBO);
	//
	// // Bind VAO and VBO/EBO
	// glBindVertexArray(VAO);
	//
	// glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
	// GL_STATIC_DRAW);
	//
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
	// 			 GL_STATIC_DRAW);
	//
	// // Set vertex attributes
	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
	// 					  (void *)0);
	// glEnableVertexAttribArray(0);
	// glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
	// 					  (void *)(3 * sizeof(float)));
	// glEnableVertexAttribArray(1);
	//
	// glBindVertexArray(0);
	//
	// Texture2D texture = Texture2D("assets/chammy.png");
	// glUniform1i(glGetUniformLocation(m_Data.m_Renderer->GetShaderProgramID(),
	// 								 "texture"),
	// 			0);
	// glActiveTexture(GL_TEXTURE0);
	// glBindTexture(GL_TEXTURE_2D, texture.GetID());

	m_Data.m_Renderer->EndBatch();
	m_Data.m_Renderer->Flush();
	camera.Update();

	m_Data.m_Renderer->PollEvents();
	// glUseProgram(shaderProgram);
	// glBindVertexArray(VAO);
	// glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	m_Data.m_Renderer->SwapBuffers();
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
