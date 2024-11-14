#pragma once

#include "Blazr/Core/Core.h"
#include <GL/glew.h>

class FrameBuffer {
  public:
	BLZR_API FrameBuffer(int width, int height);
	BLZR_API  ~FrameBuffer();

	void BLZR_API Bind() const;
	void BLZR_API Unbind() const;

	void BLZR_API Resize(int width, int height);

	GLuint BLZR_API GetTextureID() const { return m_TextureID; }
	GLuint BLZR_API GetFramebufferID() const { return m_FrameBufferID; }

	int BLZR_API GetWidth() const { return m_Width; }
	int BLZR_API GetHeight() const { return m_Height; }

  private:
	void Initialize();
	void Cleanup();

	GLuint m_FrameBufferID = 0;
	GLuint m_TextureID = 0;
	GLuint m_DepthBufferID = 0; // Optional depth buffer

	int m_Width;
	int m_Height;
};
