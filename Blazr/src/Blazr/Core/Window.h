#pragma once
#include "blzrpch.h"
#include "GL/glew.h"
#include "../../../vendor/GLFW/include/GLFW/glfw3.h"
#include "Blazr/Events/Event.h"
#include "Blazr/Renderer/Camera2D.h"
#include "Core.h"

namespace Blazr {
struct WindowProperties {
	std::string Title;
	unsigned int Width;
	unsigned int Height;

	WindowProperties(const std::string &title = "Blazr2D",
					 unsigned int width = 1280, unsigned int height = 720)
		: Title(title), Width(width), Height(height) {}
};

class BLZR_API Window {
  public:
	using EventCallbackFn = std::function<void(Event &)>;

	virtual ~Window() {}

	virtual void onUpdate() = 0;

	virtual unsigned int getWidth() const = 0;
	virtual unsigned int getHeight() const = 0;

	virtual void setEventCallback(const EventCallbackFn &callback) = 0;
	virtual void setVSync(bool enabled) = 0;
	virtual bool isVSync() const = 0;

	virtual void RenderGameScene(Camera2D camera) = 0;

	virtual GLFWwindow *GetWindow() const = 0;
	virtual Camera2D *GetCamera() = 0;

	static Window *
	create(const WindowProperties &properties = WindowProperties());
};
} // namespace Blazr
