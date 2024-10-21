#include "Blazr/Core/Core.h"
#include <GL/glew.h>
namespace Blazr {
class UniformBuffer {
  public:
	UniformBuffer(GLsizeiptr size, GLuint bindingPoint);

	~UniformBuffer();
	static Ref<UniformBuffer> Create(uint32_t size, uint32_t bindingPoint);
	void UpdateData(const void *data, uint32_t size, uint32_t offset = 0);

  private:
	GLuint m_RendererID;
};
} // namespace Blazr
