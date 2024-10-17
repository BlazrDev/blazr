#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
namespace Blazr {
class RendererAPI {

public:
  bool Init();

  void SetClearColor(float r, float g, float b, float a);

  void Clear();

  void SwapBuffers();

  void PollEvents();

  void DrawRectangle(float x, float y, float width, float height);

  bool WindowShouldClose() const;

  void Shutdown();

private:
  GLFWwindow *window = nullptr;
};

} // namespace Blazr
