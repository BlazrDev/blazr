#include "LinuxWindow.h"
#include "../../../blzrpch.h"
#include "../../Core/Core.h"
#include "../../Core/Log.h"

namespace Blazr {
static bool s_GLFWInitialized = false;
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
  m_Data.title = properties.Title;
  m_Data.width = properties.Width;
  m_Data.height = properties.Height;

  BLZR_CORE_INFO("Creating window {0} ({1}, {2})", properties.Title,
                 properties.Width, properties.Height);
  if (!s_GLFWInitialized) {
    int success = glfwInit();
    if (!success) {
      BLZR_CORE_ERROR("Could not initialize GLFW!");
      // glfwTerminate();
    }
    s_GLFWInitialized = success;
  }
  m_Window = glfwCreateWindow((int)properties.Width, (int)properties.Height,
                              properties.Title.c_str(), NULL, NULL);
  // glfwMakeContextCurrent(m_Window);
  // glfwSetWindowUserPointer(m_Window, &m_Data);
  glfwShowWindow(m_Window);
  setVSync(true);
}

void LinuxWindow::shutdown() {
  // glfwDestroyWindow(m_Window);
}

void LinuxWindow::onUpdate() {
  // glfwPollEvents();
  // glfwSwapBuffers(m_Window);
}

void LinuxWindow::setVSync(bool enabled) {
  // if (enabled)
  //   glfwSwapInterval(1);
  // else
  //   glfwSwapInterval(0);
  m_Data.vsync = enabled;
}

bool LinuxWindow::isVSync() const { return m_Data.vsync; }

void setEventCallback(const LinuxWindow::EventCallbackFn &callback) { return; }
} // namespace Blazr
