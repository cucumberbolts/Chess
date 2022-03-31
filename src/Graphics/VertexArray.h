#pragma once

#include <initializer_list>

#include "Buffer.h"

struct VertexAttribute {
    enum class Type {
        Float,
    };

    Type DataType;
    uint32_t Count;
};

class VertexArray {
public:
    VertexArray(const VertexBuffer& vertexBuffer, const std::initializer_list<VertexAttribute>& layout);
    VertexArray(const VertexArray& other) = delete;

    void Bind() const;
private:
    uint32_t m_BufferID = 0;
};
