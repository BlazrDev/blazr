#include "blzrpch.h"

#include "Blazr/Renderer/GLTexture2D.h"
#include "Blazr/Renderer/Texture.h"

namespace Blazr {
Ref<Texture2D> Texture2D::Create(const TextureSpecification &specification) {
  return CreateRef<GLTexture2D>(specification);
}

Ref<Texture2D> Texture2D::Create(const std::string &path) {
  return CreateRef<GLTexture2D>(path);
}

} // namespace Blazr
