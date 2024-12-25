#include "blzrpch.h"
#include "Blazr/Core/Log.h"
#include "Font.h"
#include "stb_truetype.h"
#include <fstream>
#include <iostream>
#include <vector>

namespace Blazr {

Font::Font(const std::string &filePath, int fontSize) : m_FontSize(fontSize) {
	LoadFont(filePath, fontSize);
	GenerateTextureAtlas();
}

void Font::LoadFont(const std::string &filePath, int fontSize) {

	std::ifstream fontFile(filePath, std::ios::binary | std::ios::ate);
	if (!fontFile) {
		throw std::runtime_error("Failed to open font file: " + filePath);
	}

	std::streamsize size = fontFile.tellg();
	fontFile.seekg(0, std::ios::beg);

	m_FontBuffer = new unsigned char[size];
	if (!fontFile.read(reinterpret_cast<char *>(m_FontBuffer), size)) {
		delete[] m_FontBuffer;
		throw std::runtime_error("Failed to read font file: " + filePath);
	}

	int fontOffset = stbtt_GetFontOffsetForIndex(m_FontBuffer, 0);
	if (!stbtt_InitFont(&m_FontInfo, m_FontBuffer, fontOffset)) {
		delete[] m_FontBuffer;
		throw std::runtime_error("Failed to initialize font");
	}
}

void Font::GenerateTextureAtlas() {
	const int atlasSize = 512;
	int padding = 2;

	std::vector<unsigned char> atlasData(atlasSize * atlasSize, 0);

	int x = padding;
	int y = padding;
	int maxRowHeight = 0;

	for (char c = 32; c < 127; ++c) {
		int width, height, xOffset, yOffset;
		unsigned char *bitmap = stbtt_GetCodepointBitmap(
			&m_FontInfo, 0, stbtt_ScaleForPixelHeight(&m_FontInfo, m_FontSize),
			c, &width, &height, &xOffset, &yOffset);

		if (x + width + padding > atlasSize) {
			x = padding;
			y += maxRowHeight + padding;
			maxRowHeight = 0;
		}

		for (int row = 0; row < height; ++row) {
			for (int col = 0; col < width; ++col) {
				atlasData[(y + row) * atlasSize + (x + col)] =
					bitmap[row * width + col];
			}
		}

		float u0 = x / static_cast<float>(atlasSize);
		float v0 = y / static_cast<float>(atlasSize);
		float u1 = (x + width) / static_cast<float>(atlasSize);
		float v1 = (y + height) / static_cast<float>(atlasSize);

		Glyph glyph;
		glyph.size = glm::vec2(width, height);
		glyph.bearing = glm::vec2(xOffset, yOffset);
		int advanceWidth, leftSideBearing;
		stbtt_GetCodepointHMetrics(&m_FontInfo, c, &advanceWidth,
								   &leftSideBearing);

		glyph.advance =
			advanceWidth * stbtt_ScaleForPixelHeight(&m_FontInfo, m_FontSize);
		glyph.bearing.x = leftSideBearing *
						  stbtt_ScaleForPixelHeight(&m_FontInfo, m_FontSize);
		glyph.uvCoords[0] = glm::vec2(u0, v0);
		glyph.uvCoords[1] = glm::vec2(u1, v0);
		glyph.uvCoords[2] = glm::vec2(u1, v1);
		glyph.uvCoords[3] = glm::vec2(u0, v1);

		m_Glyphs[c] = glyph;

		x += width + padding;
		maxRowHeight = std::max(maxRowHeight, height);

		stbtt_FreeBitmap(bitmap, nullptr);
	}

	m_TextureAtlas =
		CreateRef<Texture2D>(atlasSize, atlasSize, atlasData.data());
}

const Glyph &Font::GetGlyph(char character) const {
	return m_Glyphs.at(character);
}

const Ref<Texture2D> &Font::GetTextureAtlas() const { return m_TextureAtlas; }
float Font::GetLineHeight() const {
	int ascent, descent, lineGap;
	stbtt_GetFontVMetrics(&m_FontInfo, &ascent, &descent, &lineGap);
	return (ascent - descent + lineGap) *
		   stbtt_ScaleForPixelHeight(&m_FontInfo, m_FontSize);
}

} // namespace Blazr
