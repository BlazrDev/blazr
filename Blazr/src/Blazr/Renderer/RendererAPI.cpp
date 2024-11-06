#include "blzrpch.h"
// #include "RendererAPI.h"
// #include "Shader.h"
// #include "ShaderLoader.h"
// #include "Texture2D.h"
// #include "ext/vector_float3.hpp"
// #include <iostream>
//
// namespace Blazr {
//
// bool RendererAPI::Init() {
// 	m_ShaderProgram =
// 		ShaderLoader::Create(vertexShaderPath, fragmentShaderPath);
//
// 	m_VertexArray = VertexArray::Create();
//
// 	m_VertexBuffer = VertexBuffer::Create(MaxVertices * sizeof(Vertex));
// 	m_VertexBuffer->SetLayout({{ShaderDataType::Float3, "a_Position"},
// 							   {ShaderDataType::Float4, "a_Color"}});
// 	m_VertexArray->AddVertexBuffer(m_VertexBuffer);
//
// 	uint32_t indices[MaxIndices];
// 	uint32_t offset = 0;
// 	for (uint32_t i = 0; i < MaxIndices; i += 6) {
// 		indices[i + 0] = offset + 0;
// 		indices[i + 1] = offset + 1;
// 		indices[i + 2] = offset + 2;
//
// 		indices[i + 3] = offset + 2;
// 		indices[i + 4] = offset + 3;
// 		indices[i + 5] = offset + 0;
//
// 		offset += 4;
// 	}
// 	m_IndexBuffer = IndexBuffer::Create(indices, MaxIndices);
// 	m_VertexArray->SetIndexBuffer(m_IndexBuffer);
//
// 	// TODO remove test code for texture
// 	m_ShaderProgram->Enable();
// 	return true;
// }
//
// void RendererAPI::SetClearColor(float r, float g, float b, float a) {
// 	glClearColor(r, g, b, a);
// }
//
// void RendererAPI::Clear() { glClear(GL_COLOR_BUFFER_BIT); }
//
// void RendererAPI::BeginBatch() {
// 	m_VertexBufferData.clear();
// 	m_IndexCount = 0;
// }
//
// void RendererAPI::EndBatch() {
// 	uint32_t dataSize = (uint32_t)m_VertexBufferData.size() * sizeof(Vertex);
// 	m_VertexBuffer->SetData(m_VertexBufferData.data(), dataSize);
// }
//
// GLuint RendererAPI::GetShaderProgramID() const {
// 	return m_ShaderProgram->GetProgramID();
// }
//
// void RendererAPI::Flush() {
// 	m_VertexArray->Bind();
// 	glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, nullptr);
// }
//
// void RendererAPI::AddRectangleToBatch(float x, float y, float width,
// 									  float height, const glm::vec4 color) {
// 	if (m_IndexCount >= MaxIndices) {
// 		EndBatch();
// 		Flush();
// 		BeginBatch();
// 	}
//
// 	// m_VertexBufferData.push_back({glm::vec3(x, y, 0.0f), color});
// 	// m_VertexBufferData.push_back({glm::vec3(x + width, y, 0.0f), color});
// 	// m_VertexBufferData.push_back(
// 	// 	{glm::vec3(x + width, y + height, 0.0f), color});
// 	// m_VertexBufferData.push_back({glm::vec3(x, y + height, 0.0f), color});
//
// 	m_IndexCount += 6;
// }
//
// void RendererAPI::DrawRectangle(float x, float y, float width, float height,
// 								const glm::vec4 color) {
// 	AddRectangleToBatch(x, y, width, height, color);
// }
//
// void RendererAPI::SwapBuffers() { glfwSwapBuffers(window); }
//
// void RendererAPI::PollEvents() { glfwPollEvents(); }
//
// bool RendererAPI::WindowShouldClose() const {
// 	return glfwWindowShouldClose(window);
// }
//
// void RendererAPI::Shutdown() {
// 	glfwDestroyWindow(window);
// 	glfwTerminate();
// }
//
// void RendererAPI::CreateCamera(int width, int height) {
// 	m_Camera = Camera2D(width, height);
// }
//
// Camera2D &RendererAPI::GetCamera() { return m_Camera; }
//
// } // namespace Blazr
