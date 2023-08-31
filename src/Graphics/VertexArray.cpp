#include "VertexArray.h"

#include <glad/glad.h>

VertexArray::VertexArray(const VertexBuffer& vertexBuffer, const std::initializer_list<VertexAttribute>& layout) {
    vertexBuffer.Bind();

    glGenVertexArrays(1, &m_BufferID);
    glBindVertexArray(m_BufferID);

    int32_t stride = 0;
    for (const auto& attribute : layout) {
        switch (attribute.DataType) {
            case VertexAttribute::Type::Float: stride += sizeof(float) * attribute.Count; break;
        }
    }

    uint32_t index = 0;
    uint32_t offset = 0;

    for (const auto& attribute : layout) {
        switch (attribute.DataType) {
            case VertexAttribute::Type::Float:
            {
                glVertexAttribPointer(index, attribute.Count, GL_FLOAT, false, stride, (const void*)(intptr_t)offset);
                offset += attribute.Count * sizeof(float);
            }
        }

        glEnableVertexAttribArray(index);

        index++;
    }
}

void VertexArray::Bind() const {
    glBindVertexArray(m_BufferID);
}
