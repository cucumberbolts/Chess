#pragma once

#include "Buffer.h"
#include "Shader.h"
#include "VertexArray.h"

class Renderer {
public:
    Renderer();

    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, uint32_t vertexCount);

    void Clear();
};
