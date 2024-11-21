#include "blzrpch.h"
#include "Blazr/Core/Log.h"
#include "FrameBuffer.h"
#include <iostream>

FrameBuffer::FrameBuffer(int width, int height)
	: m_Width(width), m_Height(height) {
	Initialize();
}

FrameBuffer::~FrameBuffer() { Cleanup(); }

void FrameBuffer::Initialize() {

	glGenFramebuffers(1, &m_FrameBufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);

	glGenTextures(1, &m_TextureID);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA,
				 GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
						   m_TextureID, 0);

	glGenRenderbuffers(1, &m_DepthBufferID);
	glBindRenderbuffer(GL_RENDERBUFFER, m_DepthBufferID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width,
						  m_Height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
							  GL_RENDERBUFFER, m_DepthBufferID);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		BLZR_CORE_ERROR("Error: Framebuffer is not complete!");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Cleanup() {

	glDeleteFramebuffers(1, &m_FrameBufferID);
	glDeleteTextures(1, &m_TextureID);
	glDeleteRenderbuffers(1, &m_DepthBufferID);
}

void FrameBuffer::Bind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);
	glViewport(0, 0, m_Width, m_Height);
}

void FrameBuffer::Unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void FrameBuffer::Resize(int width, int height) {
	if (width == m_Width && height == m_Height)
		return;

	m_Width = width;
	m_Height = height;

	Cleanup();
	Initialize();
}
