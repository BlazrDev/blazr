#pragma once
#include "Buffer.h"
#include <vector>

namespace Blazr {
class VertexArray {
public:
  VertexArray();
  ~VertexArray();

  void Bind() const;
  void Unbind() const;

  void AddVertexBuffer(const Ref<VertexBuffer> &vb);
  void SetIndexBuffer(const Ref<IndexBuffer> &indexBuffer);

  const std::vector<Ref<VertexBuffer>> &GetVertexBuffers() const;
  const Ref<IndexBuffer> &GetIndexBuffer() const;

  static Ref<VertexArray> Create();

private:
  uint32_t m_RendererID;
  uint32_t m_VertexBufferIndex = 0;
  std::vector<Ref<VertexBuffer>> m_VertexBuffers;
  Ref<IndexBuffer> m_IndexBuffer;
};
} // namespace Blazr
