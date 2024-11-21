#pragma once
#include "Blazr/Core/Core.h"
#include "Blazr/Renderer/Texture2D.h"
#include "stb_truetype.h"
#include <glm.hpp>
#include <string>
#include <unordered_map>

namespace Blazr {

struct Glyph {
	glm::vec2 size;
	glm::vec2 bearing;
	float advance;
	glm::vec2 uvCoords[4];
};

class Font {
  public:
	stbtt_fontinfo m_FontInfo;
	Font(const std::string &filePath, int fontSize);
	const Glyph &GetGlyph(char character) const;
	const Ref<Texture2D> &GetTextureAtlas() const;
	float GetLineHeight() const;

  private:
	void LoadFont(const std::string &filePath, int fontSize);
	void GenerateTextureAtlas();

	std::unordered_map<char, Glyph> m_Glyphs;
	Ref<Texture2D> m_TextureAtlas;
	unsigned char *m_FontBuffer;
	int m_FontSize;
};

} // namespace Blazr
