#pragma once

#include <cstdint>

struct Vertex {
    float X;
    float Y;
};

class VertexBuffer {
public:
    VertexBuffer(Vertex* data, uint32_t count);
//private:  // For Renderer when we create one
    void Bind() const;
private:
    uint32_t m_BufferID = 0;
};

class IndexBuffer {
public:
    IndexBuffer(uint32_t* data, uint32_t count);
//private:
    void Bind() const;
private:
    uint32_t m_BufferID = 0;
};
