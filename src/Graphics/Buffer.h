#pragma once

#include <cstdint>

#include <glm/glm.hpp>

struct Vertex {
    glm::vec2 position;  // Vertex position
    glm::vec2 texCoord;  // Texture coordinates
};

class VertexBuffer {
public:
    VertexBuffer(Vertex* data, uint32_t count);
    VertexBuffer(const VertexBuffer& other) = delete;

    void Bind() const;
private:
    uint32_t m_BufferID = 0;
};

class IndexBuffer {
public:
    IndexBuffer(uint32_t* data, uint32_t count);
    IndexBuffer(const IndexBuffer& other) = delete;

    void Bind() const;
private:
    uint32_t m_BufferID = 0;
};
