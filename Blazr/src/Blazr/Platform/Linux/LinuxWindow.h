#pragma once

#include "Blazr/Core/Window.h"
#include "Blazr/Ecs/Components/SpriteComponent.h"
#include "Blazr/Ecs/Components/TransformComponent.h"
#include "Blazr/Ecs/Entity.h"
#include "Blazr/Renderer/RenderContext.h"
#include "Blazr/Renderer/Renderer2D.h"
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
	void loadTexture(const std::string &path);

  private:
	virtual void init(const WindowProperties &properties);
	virtual void shutdown();

  private:
	GLFWwindow *m_Window;
	Scope<RenderContext> m_RenderContext;
	unsigned int VAO, VBO, EBO;
	unsigned int shaderProgram;
	std::shared_ptr<Shader> m_Shader;

	struct WindowData {
		std::string title;
		unsigned int width, height;
		bool vsync;
		std::unique_ptr<Blazr::Registry> m_Registry =
			std::make_unique<Blazr::Registry>();

		EventCallbackFn eventCallback;
	};

	WindowData m_Data;
};
} // namespace Blazr
