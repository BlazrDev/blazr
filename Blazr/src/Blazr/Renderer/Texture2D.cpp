#include "blzrpch.h"
#include "Blazr/Core/Log.h"
#include "Texture2D.h"
#include "stb_image.h"
#include <iostream>

namespace Blazr {
Texture2D::Texture2D(const std::string &filepath)
	: m_FilePath(filepath), m_LocalBuffer(nullptr), m_Width(0), m_Height(0),
	  m_BPP(0) {
	stbi_set_flip_vertically_on_load(1);
	m_LocalBuffer = stbi_load(filepath.c_str(), &m_Width, &m_Height, &m_BPP, 0);

	if (!m_LocalBuffer) {
		BLZR_CORE_WARN("Failed to load texture: {0}", filepath);
		return;
	}

	GLenum internalFormat = 0;
	GLenum dataFormat = 0;

	if (m_BPP == 3) {
		internalFormat = GL_RGB8;
		dataFormat = GL_RGB;
	} else if (m_BPP == 4) {
		internalFormat = GL_RGBA8;
		dataFormat = GL_RGBA;
	} else {
		BLZR_CORE_WARN("Unsupported number of channels: {0}", m_BPP);
		return;
	}

	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0,
				 dataFormat, GL_UNSIGNED_BYTE, m_LocalBuffer);

	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (m_LocalBuffer) {
		stbi_image_free(m_LocalBuffer);
	}
}

Texture2D::~Texture2D() { glDeleteTextures(1, &m_RendererID); }

void Texture2D::Bind(uint32_t slot) const {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void Texture2D::Unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

GLuint Texture2D::GetRendererID() const { return m_RendererID; }

Ref<Texture2D> Texture2D::Create(const std::string &path) {
	return CreateRef<Texture2D>(path);
}
} // namespace Blazr
