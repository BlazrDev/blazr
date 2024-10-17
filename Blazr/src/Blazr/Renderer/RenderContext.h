#pragma once
#include "Blazr/Core/Core.h"

struct GLFWwindow;

namespace Blazr {
class RenderContext {
public:
  RenderContext(GLFWwindow *windowHandle);

  void Init();
  void SwapBuffers();

  static Scope<RenderContext> Create(void *window);

private:
  GLFWwindow *m_WindowHandle;
};
} // namespace Blazr
