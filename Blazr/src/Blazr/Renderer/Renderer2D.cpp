#include "blzrpch.h"

#include "Blazr/Ecs/Registry.h"
#include "Blazr/Resources/AssetManager.h"
#include "RenderCommand.h"
#include "Renderer2D.h"
#include "Shader.h"
#include "ShaderLoader.h"
#include "UniformBuffer.h"
#include "VertexArray.h"

namespace Blazr {
struct QuadVertex {
	glm::vec3 Position;
	glm::vec4 Color;
	glm::vec2 TexCoord;
	float TexIndex;
	float TilingFactor;
	entt::entity EntityID;
};

struct Renderer2DData {
	static const uint32_t MaxQuads = 20000;
	static const uint32_t MaxVertices = MaxQuads * 4;
	static const uint32_t MaxIndices = MaxQuads * 6;
	static const uint32_t MaxTextureSlots = 32;

	Ref<VertexArray> QuadVertexArray;
	Ref<VertexBuffer> QuadVertexBuffer;
	Ref<Shader> QuadShader;
	Ref<Texture2D> WhiteTexture;

	uint32_t QuadIndexCount = 0;
	QuadVertex *QuadVertexBufferBase = nullptr;
	QuadVertex *QuadVertexBufferPtr = nullptr;
	std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
	uint32_t TextureSlotIndex = 0;
	glm::vec4 QuadVertexPositions[4];

	struct CameraData {
		glm::mat4 ViewProjection;
	};

	CameraData CameraBuffer;
	Ref<UniformBuffer> CameraUniformBuffer;
	//
};

static Renderer2DData s_Data;

void Renderer2D::Init() {
	s_Data.QuadVertexArray = VertexArray::Create();
	s_Data.QuadVertexBuffer =
		VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));
	s_Data.QuadVertexBuffer->SetLayout(
		{{ShaderDataType::Float3, "a_Position"},
		 {ShaderDataType::Float4, "a_Color"},
		 {ShaderDataType::Float2, "a_TexCoord"},
		 {ShaderDataType::Float, "a_TexIndex"},
		 {ShaderDataType::Float, "a_TilingFactor"},
		 {ShaderDataType::Int, "a_EntityID"}});
	s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];
	s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

	uint32_t *quadIndices = new uint32_t[s_Data.MaxIndices];

	uint32_t offset = 0;
	for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6) {
		quadIndices[i + 0] = offset + 0;
		quadIndices[i + 1] = offset + 1;
		quadIndices[i + 2] = offset + 2;

		quadIndices[i + 3] = offset + 2;
		quadIndices[i + 4] = offset + 3;
		quadIndices[i + 5] = offset + 0;

		offset += 4;
	}

	Ref<IndexBuffer> quadIB =
		IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
	s_Data.QuadVertexArray->SetIndexBuffer(quadIB);
	delete[] quadIndices;

	s_Data.WhiteTexture = Texture2D::Create("assets/white_texture.png");
	s_Data.QuadShader =
		ShaderLoader::Create("shaders/vertex/TextureTestShader.vert",
							 "shaders/fragment/TextureTestShader.frag");
	s_Data.TextureSlots[0] = s_Data.WhiteTexture;
	// s_Data.QuadVertexPositions[0] = {-0.5f, -0.5f, 0.0f, 1.0f};
	// s_Data.QuadVertexPositions[1] = {0.5f, -0.5f, 0.0f, 1.0f};
	// s_Data.QuadVertexPositions[2] = {0.5f, 0.5f, 0.0f, 1.0f};
	// s_Data.QuadVertexPositions[3] = {-0.5f, 0.5f, 0.0f, 1.0f};
	//
	// Srklessove izmjenje zbog kamere da ne ide objekat izban granice
	s_Data.QuadVertexPositions[0] = {0.f, 0.f, 0.0f, 1.0f};
	s_Data.QuadVertexPositions[1] = {1.f, 0.f, 0.0f, 1.0f};
	s_Data.QuadVertexPositions[2] = {1.f, 1.f, 0.0f, 1.0f};
	s_Data.QuadVertexPositions[3] = {0.f, 1.f, 0.0f, 1.0f};

	// 		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // Bottom-left
	// 		0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, // Bottom-right
	// 		0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // Top-right
	// 		-0.5f, 0.5f,  0.0f, 0.0f, 1.0f	// Top-left

	s_Data.CameraUniformBuffer =
		UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);
	// m_Registry = std::make_unique<Registry>();
}

void Renderer2D::Shutdown() { delete[] s_Data.QuadVertexBufferBase; }

void Renderer2D::BeginScene(const Camera2D &camera) {
	StartBatch();
	s_Data.CameraBuffer.ViewProjection = camera.GetCameraMatrix();
	s_Data.CameraUniformBuffer->UpdateData(&s_Data.CameraBuffer,
										   sizeof(Renderer2DData::CameraData));
	GLuint location =
		glGetUniformLocation(s_Data.QuadShader->GetProgramID(), "uProjection");
	glUniformMatrix4fv(location, 1, GL_FALSE,
					   &s_Data.CameraBuffer.ViewProjection[0][0]);
}

void Renderer2D::EndScene() { Flush(); }

void Renderer2D::StartBatch() {
	s_Data.QuadIndexCount = 0;
	s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
	s_Data.TextureSlotIndex = 1;
}
void Renderer2D::Flush() {
	if (s_Data.QuadIndexCount) {
		uint32_t dataSize = (uint32_t)((uint8_t *)s_Data.QuadVertexBufferPtr -
									   (uint8_t *)s_Data.QuadVertexBufferBase);
		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++) {
			s_Data.TextureSlots[i]->Bind(i);
		}

		s_Data.QuadShader->Enable();
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray,
								   s_Data.QuadIndexCount);
	}
}

void Renderer2D::NextBatch() {
	Flush();
	StartBatch();
}

void Renderer2D::DrawQuad(entt::entity entityID, const glm::vec2 &position,
						  const glm::vec2 &size, const glm::vec4 &color) {

	constexpr size_t quadVertexCount = 4;
	const float textureIndex = 0.f;
	constexpr glm::vec2 textureCoords[] = {
		{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f}};
	const float tilingFactor = 1.f;

	if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices) {
		NextBatch();
	}

	glm::vec3 pos = {position.x, position.y, 0.0f};
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) *
						  glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

	for (size_t i = 0; i < quadVertexCount; i++) {
		s_Data.QuadVertexBufferPtr->Position =
			transform * s_Data.QuadVertexPositions[i];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->EntityID = entityID;
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;
	}

	s_Data.QuadIndexCount += 6;
}
void Renderer2D::DrawQuad(entt::entity entityID, const glm::vec2 &position,
						  const glm::vec2 &size, const Ref<Texture2D> &texture,
						  float tilingFactor, const glm::vec4 &tintColor) {

	constexpr size_t quadVertexCount = 4;
	constexpr glm::vec2 textureCoords[] = {
		{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f}};
	if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices) {
		NextBatch();
	}

	float textureIndex = 0.f;

	for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++) {
		if (*s_Data.TextureSlots[i] == *texture) {
			textureIndex = (float)i;
			break;
		}
	}

	if (textureIndex == 0.f) {
		if (s_Data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots) {
			NextBatch();
		}

		textureIndex = (float)s_Data.TextureSlotIndex;
		s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
		s_Data.TextureSlotIndex++;
	}

	glm::vec3 pos = {position.x, position.y, 0.0f};
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) *
						  glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

	for (size_t i = 0; i < quadVertexCount; i++) {
		s_Data.QuadVertexBufferPtr->Position =
			transform * s_Data.QuadVertexPositions[i];
		s_Data.QuadVertexBufferPtr->Color = tintColor;
		s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;
	}

	s_Data.QuadIndexCount += 6;
}

void Renderer2D::DrawQuad(entt::entity entityID, const glm::vec2 &position,
						  const glm::vec2 &size, float rotation,
						  const glm::vec2 &scale, const Ref<Texture2D> &texture,
						  float tilingFactor, const glm::vec4 &tintColor) {

	constexpr size_t quadVertexCount = 4;
	constexpr glm::vec2 textureCoords[] = {
		{0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f}};
	if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices) {
		NextBatch();
	}

	float textureIndex = 0.f;

	for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++) {
		if (*s_Data.TextureSlots[i] == *texture) {
			textureIndex = (float)i;
			break;
		}
	}

	if (textureIndex == 0.f) {
		if (s_Data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots) {
			NextBatch();
		}

		textureIndex = (float)s_Data.TextureSlotIndex;
		s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
		s_Data.TextureSlotIndex++;
	}

	glm::vec3 pos = {position.x, position.y, 0.0f};

	// Apply rotation and scale
	glm::mat4 transform =
		glm::translate(glm::mat4(1.0f), pos) *
		glm::rotate(glm::mat4(1.0f), glm::radians(rotation),
					{0.0f, 0.0f, 1.0f}) *
		glm::scale(glm::mat4(1.0f), {scale.x * size.x, scale.y * size.y, 1.0f});

	for (size_t i = 0; i < quadVertexCount; i++) {
		s_Data.QuadVertexBufferPtr->Position =
			transform * s_Data.QuadVertexPositions[i];
		s_Data.QuadVertexBufferPtr->Color = tintColor;
		s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;
	}

	s_Data.QuadIndexCount += 6;
}

void Renderer2D::DrawQuad(Registry &registry, entt::entity entityID) {
	// Retrieve the components associated with this entity
	auto &transform = registry.GetRegistry().get<TransformComponent>(entityID);
	auto &sprite = registry.GetRegistry().get<SpriteComponent>(entityID);

	// Assume TransformComponent contains a position and a size
	glm::vec2 position = transform.position;
	glm::vec2 size = {sprite.width, sprite.height};

	// Assume SpriteComponent contains a texture, tiling factor, and tint color
	auto assetManager = AssetManager::GetInstance();
	Ref<Texture2D> texture = assetManager->GetTexture(sprite.texturePath);
	// float tilingFactor = sprite.TilingFactor;
	// glm::vec4 tintColor = sprite.TintColor;

	// Render the quad using the other DrawQuad method
	DrawQuad(entityID, position, size, transform.rotation, transform.scale,
			 texture, 1.0f, sprite.color);
}

void Renderer2D::Clear() { glClear(GL_COLOR_BUFFER_BIT); }

} // namespace Blazr
