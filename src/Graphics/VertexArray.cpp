#include "VertexArray.h"

#include <glad/glad.h>

VertexArray::VertexArray(VertexBuffer vertexBuffer, const std::initializer_list<VertexAttribute>& layout) {
    vertexBuffer.Bind();

    glGenVertexArrays(1, &m_BufferID);
    glBindVertexArray(m_BufferID);

    uint32_t index = 0;
    uint32_t offset = 0;

    for (const auto& attribute : layout) {
        switch (attribute.DataType) {
            case VertexAttribute::Type::Float: {
                glVertexAttribPointer(index, attribute.Count, GL_FLOAT, false, attribute.Count * sizeof(float), (const void*)offset);
            }
        }

        glEnableVertexAttribArray(index);

        index++;
    }
}

void VertexArray::Bind() const {
    glBindVertexArray(m_BufferID);
}
