#include "Buffer.h"

#include <glad/glad.h>

VertexBuffer::VertexBuffer(Vertex* data, uint32_t count) {
    glGenBuffers(1, &m_BufferID);
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(Vertex), data, GL_STATIC_DRAW);
}

void VertexBuffer::Bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
}




IndexBuffer::IndexBuffer(uint32_t* data, uint32_t count) {
    glGenBuffers(1, &m_BufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), data, GL_STATIC_DRAW);
}

void IndexBuffer::Bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
}
