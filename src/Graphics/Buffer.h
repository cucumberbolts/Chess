#pragma once

#include <cstdint>

class VertexBuffer {
public:
    VertexBuffer(size_t size);
    VertexBuffer(void* data, size_t size);
    VertexBuffer(const VertexBuffer&) = delete;

    VertexBuffer& operator=(const VertexBuffer&) = delete;

    void Bind() const;

    void SetData(void* data, size_t size);
private:
    uint32_t m_BufferID = 0;
};

class IndexBuffer {
public:
    IndexBuffer(size_t count);
    IndexBuffer(uint32_t* data, uint32_t count);
    IndexBuffer(const IndexBuffer&) = delete;

    IndexBuffer& operator=(const IndexBuffer&) = delete;

    void Bind() const;

    uint32_t GetCount() const { return m_Count; }

    void SetData(uint32_t* data, uint32_t count);
private:
    uint32_t m_BufferID = 0;

    uint32_t m_Count;
};
