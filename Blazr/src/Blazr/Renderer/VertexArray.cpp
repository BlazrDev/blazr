#include "blzrpch.h"
#include "VertexArray.h"
#include <GL/glew.h>

namespace Blazr {

static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type) {
  switch (type) {
  case ShaderDataType::Float:
    return GL_FLOAT;
  case ShaderDataType::Float2:
    return GL_FLOAT;
  case ShaderDataType::Float3:
    return GL_FLOAT;
  case ShaderDataType::Float4:
    return GL_FLOAT;
  case ShaderDataType::Mat3:
    return GL_FLOAT;
  case ShaderDataType::Mat4:
    return GL_FLOAT;
  case ShaderDataType::Int:
    return GL_INT;
  case ShaderDataType::Int2:
    return GL_INT;
  case ShaderDataType::Int3:
    return GL_INT;
  case ShaderDataType::Int4:
    return GL_INT;
  case ShaderDataType::Bool:
    return GL_BOOL;
  default:
    BLZR_CORE_ERROR("Unknown ShaderDataType!");
    return 0;
  }
}

VertexArray::VertexArray() { 
    glCreateVertexArrays(1, &m_RendererID); 
}

VertexArray::~VertexArray() { 
    glDeleteVertexArrays(1, &m_RendererID); 
}

void VertexArray::Bind() const { 
    glBindVertexArray(m_RendererID); 
}

void VertexArray::Unbind() const {
    glBindVertexArray(0); // 0 unbinds any currently bound VAO
}

void VertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vb) {
    Bind();
    vb->Bind();

    if (vb->GetLayout().GetElements().size() <= 0) {
        BLZR_CORE_ERROR("Vertex Buffer has no layout!");
        return;
    }
    
    const auto& layout = vb->GetLayout();
    const auto& elements = layout.GetElements();
    for (const auto& element : elements) {
        switch (element.Type) {
        case ShaderDataType::Float:
        case ShaderDataType::Float2:
        case ShaderDataType::Float3:
        case ShaderDataType::Float4: {
            glEnableVertexAttribArray(m_VertexBufferIndex);
            glVertexAttribPointer(m_VertexBufferIndex, element.GetComponentCount(),
                                  ShaderDataTypeToOpenGLBaseType(element.Type),
                                  element.Normalized ? GL_TRUE : GL_FALSE,
                                  layout.GetStride(), (const void*)element.Offset);
            m_VertexBufferIndex++;
            break;
        }
        case ShaderDataType::Int:
        case ShaderDataType::Int2:
        case ShaderDataType::Int3:
        case ShaderDataType::Int4:
        case ShaderDataType::Bool: {
            glEnableVertexAttribArray(m_VertexBufferIndex);
            glVertexAttribIPointer(m_VertexBufferIndex, element.GetComponentCount(),
                                   ShaderDataTypeToOpenGLBaseType(element.Type),
                                   layout.GetStride(), (const void*)element.Offset);
            m_VertexBufferIndex++;
            break;
        }
        case ShaderDataType::Mat3:
        case ShaderDataType::Mat4: {
            uint8_t count = element.GetComponentCount();
            for (uint8_t i = 0; i < count; i++) {
                glEnableVertexAttribArray(m_VertexBufferIndex);
                glVertexAttribPointer(
                    m_VertexBufferIndex, count,
                    ShaderDataTypeToOpenGLBaseType(element.Type),
                    element.Normalized ? GL_TRUE : GL_FALSE, layout.GetStride(),
                    (const void*)(element.Offset + sizeof(float) * count * i));
                glVertexAttribDivisor(m_VertexBufferIndex, 1);
                m_VertexBufferIndex++;
            }
            break;
        }
        default:
            BLZR_CORE_ERROR("Unknown ShaderDataType!");
        }
    }

    m_VertexBuffers.push_back(vb);
}

void VertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) {
    Bind(); // Bind the vertex array
    indexBuffer->Bind(); // Ensure the index buffer is bound
    m_IndexBuffer = indexBuffer; // Store the index buffer
}

const std::vector<Ref<VertexBuffer>>& VertexArray::GetVertexBuffers() const {
    return m_VertexBuffers;
}

const Ref<IndexBuffer>& VertexArray::GetIndexBuffer() const {
    return m_IndexBuffer;
}

Ref<VertexArray> VertexArray::Create() {
    return CreateRef<VertexArray>();
}

} // namespace Blazr