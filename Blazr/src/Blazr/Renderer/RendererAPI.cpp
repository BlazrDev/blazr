#include "RendererAPI.h"
#include <iostream>

namespace Blazr {
bool RendererAPI::Init() {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return false;
  }

  window = glfwCreateWindow(800, 600, "OpenGL Renderer", nullptr, nullptr);
  if (!window) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return false;
  }

  glfwMakeContextCurrent(window);

  if (glewInit() != GLEW_OK) {
    std::cerr << "Failed to initialize GLEW" << std::endl;
    return false;
  }

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  return true;
}

void RendererAPI::SetClearColor(float r, float g, float b, float a) {
  glClearColor(r, g, b, a);
}

void RendererAPI::Clear() { glClear(GL_COLOR_BUFFER_BIT); }

void RendererAPI::SwapBuffers() { glfwSwapBuffers(window); }

void RendererAPI::PollEvents() { glfwPollEvents(); }

void RendererAPI::DrawRectangle(float x, float y, float width, float height) {
  glBegin(GL_QUADS);
  glVertex2f(x, y);
  glVertex2f(x + width, y);
  glVertex2f(x + width, y + height);
  glVertex2f(x, y + height);
  glEnd();
}

bool RendererAPI::WindowShouldClose() const {
  return glfwWindowShouldClose(window);
}

void RendererAPI::Shutdown() {
  glfwDestroyWindow(window);
  glfwTerminate();
}
} // namespace Blazr
