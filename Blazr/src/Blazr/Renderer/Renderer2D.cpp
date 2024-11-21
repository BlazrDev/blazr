#include "blzrpch.h"

#include "Blazr/Ecs/Registry.h"
#include "Blazr/Resources/AssetManager.h"
#include "RenderCommand.h"
#include "Renderer2D.h"
#include "Shader.h"
#include "ShaderLoader.h"
#include "UniformBuffer.h"
#include "VertexArray.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace Blazr {
fs::path baseAssetPath = fs::path("assets");
fs::path shaderPath = fs::path("shaders");
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

	s_Data.WhiteTexture =
		Texture2D::Create((baseAssetPath / "white_texture.png").string());
	s_Data.QuadShader = ShaderLoader::Create(
		(shaderPath / "vertex" / "TextureTestShader.vert").string(),
		(shaderPath / "fragment" / "TextureTestShader.frag").string());

	s_Data.TextureSlots[0] = s_Data.WhiteTexture;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	s_Data.QuadVertexPositions[0] = {0.f, 0.f, 0.0f, 1.0f};
	s_Data.QuadVertexPositions[1] = {1.f, 0.f, 0.0f, 1.0f};
	s_Data.QuadVertexPositions[2] = {1.f, 1.f, 0.0f, 1.0f};
	s_Data.QuadVertexPositions[3] = {0.f, 1.f, 0.0f, 1.0f};

	s_Data.CameraUniformBuffer =
		UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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
	constexpr int x = 0, y = 0;
	constexpr float spriteWidth = 472.f, spriteHeight = 617.f;
	float textureWidth = texture->GetWidth(),
		  textureHeight = texture->GetHeight();

	// glm::vec2 textureCoords[] = {
	// 	{(x * spriteWidth) / textureWidth,
	// 	 ((y + 1) * spriteHeight) / textureHeight},
	// 	{((x + 1) * spriteWidth) / textureWidth,
	// 	 ((y + 1) * spriteHeight) / textureHeight},
	// 	{((x + 1) * spriteWidth) / textureWidth,
	// 	 (y * spriteHeight) / textureHeight},
	// 	{(x * spriteWidth) / textureWidth, (y * spriteHeight) / textureHeight}};

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

	constexpr int x = 0, y = 0;
	constexpr float spriteWidth = 472.f, spriteHeight = 617.f;
	float textureWidth = texture->GetWidth(),
		  textureHeight = texture->GetHeight();

	constexpr size_t quadVertexCount = 4;

	//
	glm::vec2 textureCoords[] = {
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

	glm::mat4 transform =
		glm::translate(glm::mat4(1.0f), pos) *
		glm::rotate(glm::mat4(1.0f), glm::radians(rotation),
					{0.0f, 0.0f, 1.0f}) *
		glm::scale(glm::mat4(1.0f),
				   {scale.x * scale.x, scale.y * scale.y, 1.0f});

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
						  const glm::vec2 &size, const Ref<Texture2D> &texture,
						  float rotation, const glm::vec2 &scale,
						  float tilingFactor, const glm::vec4 &tintColor,
						  const glm::vec2 *textureCoords) {

	constexpr int x = 0, y = 0;
	constexpr float spriteWidth = 472.f, spriteHeight = 617.f;
	float textureWidth = texture->GetWidth(),
		  textureHeight = texture->GetHeight();

	constexpr size_t quadVertexCount = 4;

	//

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

	glm::mat4 transform =
		glm::translate(glm::mat4(1.0f), pos) *
		glm::rotate(glm::mat4(1.0f), glm::radians(rotation),
					{0.0f, 0.0f, 1.0f}) *
		glm::scale(glm::mat4(1.0f), {size.x * scale.x, size.y * scale.y, 1.0f});

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
	auto &transform = registry.GetRegistry().get<TransformComponent>(entityID);
	auto &sprite = registry.GetRegistry().get<SpriteComponent>(entityID);

	glm::vec2 position = transform.position;
	glm::vec2 size = {sprite.width, sprite.height};

	auto assetManager = AssetManager::GetInstance();
	Ref<Texture2D> texture = assetManager->GetTexture(sprite.texturePath);

	DrawQuad(entityID, position, size, texture, transform.rotation,
			 transform.scale, 1.0f, sprite.color, sprite.textureCoordinates);
	// DrawQuad(entityID, position, size, texture, 1.0f, sprite.color);
}

void Renderer2D::Clear() { glClear(GL_COLOR_BUFFER_BIT); }

void Renderer2D::DrawText(const std::string &text, const glm::vec2 &position,
						  const glm::vec4 &color, const Ref<Font> &font,
						  float scale, float maxWidth) {
	glm::vec2 pos = position;
	float lineStartX = position.x;

	std::istringstream stream(text);
	std::string word;
	float spaceWidth = font->GetGlyph(' ').advance * scale;

	while (stream >> word) {

		float wordWidth = 0.0f;
		for (size_t i = 0; i < word.size(); ++i) {
			const Glyph &glyph = font->GetGlyph(word[i]);
			wordWidth += glyph.advance * scale;

			if (i + 1 < word.size()) {
				float kerning = stbtt_GetCodepointKernAdvance(
									&font->m_FontInfo, word[i], word[i + 1]) *
								scale;
				wordWidth += kerning;
			}
		}

		if (pos.x + wordWidth > lineStartX + maxWidth) {
			pos.x = lineStartX;
			pos.y += font->GetLineHeight() * scale;
		}
		std::unordered_map<std::string, float> customKerning = {
			{"To", 2.0f}, {"AV", 1.5f}, {"of", 7.9f},
			{"ap", 4.2f}, {"pp", 3.2f},
		};

		for (size_t i = 0; i < word.size(); ++i) {
			char character = word[i];
			const Glyph &glyph = font->GetGlyph(character);

			glm::vec2 charPos =
				pos + glm::vec2(glyph.bearing.x * scale,
								(glyph.bearing.y > 0 ? -glyph.bearing.y
													 : glyph.bearing.y) *
									scale);
			RenderCharacter(glyph, charPos, color, scale,
							font->GetTextureAtlas());

			pos.x += glyph.advance * scale;

			float kerning = stbtt_GetCodepointKernAdvance(
				&font->m_FontInfo, character, word[i + 1]);
			if (i + 1 < word.size()) {
				std::string pair =
					std::string(1, character) + std::string(1, word[i + 1]);
				if (customKerning.find(pair) != customKerning.end()) {
					pos.x += customKerning[pair] * scale;
				}
			}
		}

		pos.x += spaceWidth;
	}
}

void Renderer2D::RenderCharacter(const Glyph &glyph, const glm::vec2 &position,
								 const glm::vec4 &color, float scale,
								 const Ref<Texture2D> &textureAtlas) {
	glm::vec2 quadSize = glyph.size * scale;

	DrawQuad(position, quadSize, textureAtlas, glyph.uvCoords, color);
}

void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size,
						  const Ref<Texture2D> &texture,
						  const glm::vec2 uvCoords[4], const glm::vec4 &color) {
	constexpr size_t quadVertexCount = 4;

	if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices) {
		NextBatch();
	}

	float textureIndex = 0.0f;
	for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++) {
		if (*s_Data.TextureSlots[i] == *texture) {
			textureIndex = (float)i;
			break;
		}
	}

	if (textureIndex == 0.0f) {
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
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = uvCoords[i];
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = 1.0f;
		s_Data.QuadVertexBufferPtr++;
	}

	s_Data.QuadIndexCount += 6;
}

} // namespace Blazr
