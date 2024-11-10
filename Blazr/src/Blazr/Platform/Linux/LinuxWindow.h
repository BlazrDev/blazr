#pragma once

#include "Blazr/Core/Window.h"
#include "Blazr/Renderer/RenderContext.h"
#include "Blazr/Renderer/Renderer2D.h"
#include "Blazr/Renderer/Shader.h"
#include <memory>

namespace Blazr {
class BLZR_API LinuxWindow : public Window {
  public:
	LinuxWindow(const WindowProperties &properties);
	virtual ~LinuxWindow() override;

	void onUpdate() override;
	unsigned int getWidth() const override;
	unsigned int getHeight() const override;
	void setEventCallback(const EventCallbackFn &callback) override;
	void setVSync(bool enabled) override;
	bool isVSync() const override;
	GLFWwindow *GetWindow() const override;
	Camera2D *GetCamera() override;

  private:
	virtual void init(const WindowProperties &properties);
	virtual void shutdown();
	GLFWwindow *m_Window;
	Scope<RenderContext> m_RenderContext;
	unsigned int VAO, VBO, EBO;
	unsigned int shaderProgram;
	std::shared_ptr<Shader> m_Shader;

	struct WindowData {
		Camera2D m_Camera{1280, 720};
		std::string title;
		unsigned int width, height;
		bool vsync;
		std::unique_ptr<Blazr::Registry> m_Registry =
			std::make_unique<Blazr::Registry>();

		EventCallbackFn eventCallback;

		glm::vec2 GetWorldCoordinates(double mouseX, double mouseY,
									  const Camera2D &camera, int screenWidth,
									  int screenHeight) {
			// Normalize screen coordinates to (-1, 1)
			glm::vec2 normalizedCoords = {
				2.0f * (mouseX / screenWidth) - 1.0f, // X coordinate
				1.0f - 2.0f * (mouseY / screenHeight) // Y coordinate
			};

			// Inverse the camera's view-projection matrix to get world
			// coordinates
			glm::mat4 inverseViewProj =
				glm::inverse(camera.GetOrthoProjection());
			glm::vec4 worldCoords =
				inverseViewProj * glm::vec4(normalizedCoords, 0.0f, 1.0f);

			return glm::vec2(worldCoords.x, worldCoords.y);
		}

		// TODO: napraviti da radi i za scale kamere, posto kada je mala slika
		// pozicija entity je ista kao da su tek kreirani
		bool IsMouseOverEntity(
			const glm::vec2 &mouseWorldPos, const glm::vec2 &entityPos,
			const glm::vec2 &entitySize, // Entity size (width, height)
			const glm::vec2 &entityScale, const float &cameraScale) {

			// Calculate scaled size based on entity's scale and camera's scale
			glm::vec2 scaledSize = entitySize * entityScale * cameraScale;

			// Adjust the entity position based on camera scale
			glm::vec2 adjustedEntityPos;
			if (cameraScale != 1.0f) {
				adjustedEntityPos =
					entityPos * cameraScale; // Adjust position for camera scale
			} else {
				adjustedEntityPos =
					entityPos; // Use original position if no scaling
			}

			// Calculate the bounding box
			glm::vec2 minBounds = adjustedEntityPos - scaledSize / 2.0f;
			glm::vec2 maxBounds = adjustedEntityPos + scaledSize / 2.0f;

			// Check if the mouse is within the bounds of the entity
			return mouseWorldPos.x > minBounds.x &&
				   mouseWorldPos.x < maxBounds.x &&
				   mouseWorldPos.y > minBounds.y &&
				   mouseWorldPos.y < maxBounds.y;
		}
	};

	WindowData m_Data;
};
} // namespace Blazr
