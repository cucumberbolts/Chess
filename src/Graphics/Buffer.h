#pragma once

#include <cstdint>

struct Vertex {
    float X;
    float Y;
    float TextureCoordsX;
    float TextureCoordsY;
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
