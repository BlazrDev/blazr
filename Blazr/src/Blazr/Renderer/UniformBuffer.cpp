#include "blzrpch.h"
#include "UniformBuffer.h"
namespace Blazr {
UniformBuffer::UniformBuffer(GLsizeiptr size, GLuint bindingPoint) {
	glGenBuffers(1, &m_RendererID);
	glNamedBufferData(m_RendererID, size, nullptr, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, m_RendererID);
}

UniformBuffer::~UniformBuffer() { glDeleteBuffers(1, &m_RendererID); }

void UniformBuffer::UpdateData(const void *data, uint32_t size,
							   uint32_t offset) {
	glBufferSubData(m_RendererID, offset, size, data);
}

Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t bindingPoint) {
	return CreateRef<UniformBuffer>(size, bindingPoint);
}

} // namespace Blazr
