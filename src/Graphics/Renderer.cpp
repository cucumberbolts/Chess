#include "Renderer.h"

#include <glad/glad.h>

Renderer::Renderer() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, uint32_t vertexCount) {
    va.Bind();
    ib.Bind();
    shader.Bind();

    glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, nullptr);
}

void Renderer::Clear() {
    glClear(GL_COLOR_BUFFER_BIT);
}
