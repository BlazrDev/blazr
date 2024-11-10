#include "blzrpch.h"
#include "RenderCommand.h"

namespace Blazr {
void OpenGLMessageCallback(unsigned source, unsigned type, unsigned id,
						   unsigned severity, int length, const char *message,
						   const void *userParam) {
	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:
		BLZR_CORE_CRITICAL(message);
		return;
	case GL_DEBUG_SEVERITY_MEDIUM:
		BLZR_CORE_ERROR(message);
		return;
	case GL_DEBUG_SEVERITY_LOW:
		BLZR_CORE_WARN(message);
		return;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		BLZR_CORE_TRACE(message);
		return;
	}
}

void RenderCommand::Init() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
}

void RenderCommand::SetViewport(uint32_t x, uint32_t y, uint32_t width,
								uint32_t height) {
	glViewport(x, y, width, height);
}

void RenderCommand::SetClearColor(const glm::vec4 &color) {
	glClearColor(color.r, color.g, color.b, color.a);
}

void RenderCommand::Clear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderCommand::DrawIndexed(const Ref<VertexArray> &vertexArray,
								uint32_t indexCount) {
	vertexArray->Bind();
	uint32_t count =
		indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
}

void RenderCommand::DrawLines(const Ref<VertexArray> &vertexArray,
							  uint32_t vertexCount) {
	vertexArray->Bind();
	glDrawArrays(GL_LINES, 0, vertexCount);
}
} // namespace Blazr
