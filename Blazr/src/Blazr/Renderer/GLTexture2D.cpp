#include "blzrpch.h"
#include "Blazr/Core/Log.h"
#include "Blazr/Renderer/GLTexture2D.h"

#include "stb_image.h"

namespace Blazr {
namespace Utils {
static GLenum BlazrImageFormatToGLDataFormat(ImageFormat format) {
  switch (format) {
  case ImageFormat::RGB8:
    return GL_RGB;
  case ImageFormat::RGBA8:
    return GL_RGBA;
  }
  BLZR_CORE_ERROR("Error getting image format!");
  return 0;
}

static GLenum BlzrImageFormatToGLInternalFormat(ImageFormat format) {
  switch (format) {
  case ImageFormat::RGB8:
    return GL_RGB;
  case ImageFormat::RGBA8:
    return GL_RGBA;
  }
  BLZR_CORE_ERROR("Error getting image format!");
  return 0;
}
} // namespace Utils

GLTexture2D::GLTexture2D(const TextureSpecification &specification)
    : m_Specification(specification), m_Width(m_Specification.Width),
      m_Height(m_Specification.Height) {}
} // namespace Blazr
