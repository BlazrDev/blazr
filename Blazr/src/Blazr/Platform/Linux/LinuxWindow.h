#pragma once

#include "Blazr/Core/Window.h"
#include "Blazr/Ecs/Components/SpriteComponent.h"
#include "Blazr/Ecs/Components/TransformComponent.h"
#include "Blazr/Ecs/Entity.h"
#include "Blazr/Renderer/RenderContext.h"
#include "Blazr/Renderer/RendererAPI.h"

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

  private:
	virtual void init(const WindowProperties &properties);
	virtual void shutdown();

  private:
	GLFWwindow *m_Window;
	Scope<RenderContext> m_RenderContext;
	unsigned int VAO;
	unsigned int shaderProgram;

	struct WindowData {
		std::string title;
		unsigned int width, height;
		bool vsync;
		RendererAPI *m_Renderer;
		std::unique_ptr<Blazr::Registry> m_Registry =
			std::make_unique<Blazr::Registry>();

		EventCallbackFn eventCallback;
		void createRect(float x, float y) {
			Camera2D camera(800.0f, 600.0f);
			camera.SetPosition(glm::vec2(0.0f, 0.0f));
			camera.SetScale(1.0f);
			auto registry = std::make_unique<Blazr::Registry>();
			Entity entity = Entity(*registry, "Ent1", "G1");
			m_Renderer->BeginBatch();
			auto &transform =
				entity.AddComponent<TransformComponent>(TransformComponent{
					.position = glm::vec2(2.0f * (x / width) - 1.0f,
										  1.0f - 2.0f * (y / height)),
					.scale = glm::vec2(1.0f, 1.0f),
					.rotation = 0.0f});
			auto &sprite = entity.AddComponent<SpriteComponent>(SpriteComponent{
				.width = 0.2f, .height = 0.2f, .startX = 0, .startY = 0});
			m_Renderer->DrawRectangle(transform.position.x - sprite.width / 2,
									  transform.position.y - sprite.height / 2,
									  sprite.width, sprite.height,
									  {1.0f, 0.0f, 0.0f, 1.0f});
			m_Renderer->EndBatch();
			m_Renderer->Flush();
			m_Renderer->SwapBuffers();
			m_Renderer->PollEvents();
		}
	};

	WindowData m_Data;
};
} // namespace Blazr
