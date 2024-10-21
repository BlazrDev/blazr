#pragma once
#include "Blazr/Core/Core.h"
#include <GL/glew.h>
#include <string>

namespace Blazr {

class Texture2D {
  public:
	Texture2D(const std::string &filepath);
	~Texture2D();

	void Bind(uint32_t slot = 0) const;
	void Unbind() const;

	int GetWidth() const { return m_Width; }
	int GetHeight() const { return m_Height; }
	GLuint GetRendererID() const;
	uint32_t GetID() const { return m_RendererID; }
	bool operator==(const Texture2D &other) const {
		return m_RendererID == other.GetRendererID();
	}

	static Ref<Texture2D> Create(const std::string &filepath);

  private:
	std::string m_FilePath;
	uint32_t m_RendererID;
	int m_Width, m_Height, m_BPP; // BPP = Bits Per Pixel
	unsigned char *m_LocalBuffer;
};
} // namespace Blazr
