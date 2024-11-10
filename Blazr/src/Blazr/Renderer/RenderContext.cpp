#include "blzrpch.h"
#include "RenderContext.h"

#include "GL/glew.h"
#include "../../../vendor/GLFW/include/GLFW/glfw3.h"
#include "Blazr/Core/Log.h"

namespace Blazr {
RenderContext::RenderContext(GLFWwindow *windowHandle)
    : m_WindowHandle(windowHandle) {
  if (windowHandle == NULL) {
    BLZR_CORE_ERROR("Window handle is null!");
  }
}

void RenderContext::Init() {
  glfwMakeContextCurrent(m_WindowHandle);
  glewExperimental = GL_TRUE;
  GLenum err = glewInit();
  if (err != GLEW_OK) {
    BLZR_CORE_ERROR("Failed to initialize GLEW! Error: {0}",
                    reinterpret_cast<const char *>(glewGetErrorString(err)));
    return;
  }
  BLZR_CORE_INFO("OpenGL Info:");
  BLZR_CORE_INFO("  Vendor: {0}",
                 reinterpret_cast<const char *>(glGetString(GL_VENDOR)));
  BLZR_CORE_INFO("  Renderer: {0}",
                 reinterpret_cast<const char *>(glGetString(GL_RENDERER)));
  BLZR_CORE_INFO("  Version: {0}",
                 reinterpret_cast<const char *>(glGetString(GL_VERSION)));

  if (!(GLEW_VERSION_4_5)) {
    BLZR_CORE_ERROR("Blazr requires at least OpenGL version 4.5!");
    return;
  }
}

Scope<RenderContext> RenderContext::Create(void *window) {
  return CreateScope<RenderContext>(static_cast<GLFWwindow *>(window));
}

void RenderContext::SwapBuffers() { glfwSwapBuffers(m_WindowHandle); }
} // namespace Blazr
