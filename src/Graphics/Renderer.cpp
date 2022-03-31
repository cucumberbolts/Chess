#include "Renderer.h"

#include <iostream>
#include <glad/glad.h>

static constexpr size_t maxQuadCount = 100;
static constexpr size_t maxVertexCount = maxQuadCount * 4;
static constexpr size_t maxIndexCount = maxQuadCount * 6;
static constexpr size_t maxTextureCount = 32;

static constexpr std::initializer_list<VertexAttribute> s_VertexLayout = {
    { VertexAttribute::Type::Float, 2 },
    { VertexAttribute::Type::Float, 4 },
    { VertexAttribute::Type::Float, 2 },
    { VertexAttribute::Type::Float, 1 },
};

Renderer::Renderer()
    : m_IndexBuffer(maxIndexCount), m_VertexBuffer(maxVertexCount), m_VertexArray(m_VertexBuffer, s_VertexLayout), m_WhiteTexture(1, 1, 0) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_Vertecies = new Vertex[maxVertexCount];
    m_Indicies = new uint32_t[maxIndexCount];

    m_NextVertex = m_Vertecies;
    m_NextIndex = m_Indicies;

    constexpr uint32_t whitePixel = 0xFFFFFFFF;
    m_WhiteTexture.SetData((uint8_t*)&whitePixel);
}

Renderer::~Renderer() {
    delete[] m_Vertecies;
    delete[] m_Indicies;
}

void Renderer::DrawRect(glm::vec3 position, glm::vec2 size, glm::vec4 colour) {
    const uint32_t vertexCount = m_NextVertex - m_Vertecies;

    if (vertexCount == maxQuadCount)
        Flush();

    m_NextVertex->Position = { position.x, position.y - size.y };
    m_NextVertex->Colour = colour;
    m_NextVertex->TexCoord = { 0.0f, 0.0f };
    m_NextVertex->TextureSlot = 0.0f;
    m_NextVertex++;

    m_NextVertex->Position = { position.x + size.x, position.y - size.y };
    m_NextVertex->Colour = colour;
    m_NextVertex->TexCoord = { 1.0f, 0.0f };
    m_NextVertex->TextureSlot = 0.0f;
    m_NextVertex++;

    m_NextVertex->Position = { position.x + size.x, position.y };
    m_NextVertex->Colour = colour;
    m_NextVertex->TexCoord = { 1.0f, 1.0f };
    m_NextVertex->TextureSlot = 0.0f;
    m_NextVertex++;

    m_NextVertex->Position = { position.x, position.y };
    m_NextVertex->Colour = colour;
    m_NextVertex->TexCoord = { 0.0f, 1.0f };
    m_NextVertex->TextureSlot = 0.0f;
    m_NextVertex++;

    static constexpr uint32_t indicies[] = {
        0, 1, 2,
        2, 3, 0,

        4, 5, 6,
        6, 7, 4,
    };

    for (size_t i = 0; i < 6; i++) {
        *m_NextIndex = vertexCount + indicies[i];
        m_NextIndex++;
    }
}

void Renderer::DrawRect(glm::vec3 position, glm::vec2 size, Texture& texture) {
    const uint32_t vertexCount = m_NextVertex - m_Vertecies;

    if (vertexCount == maxQuadCount)
        Flush();

    if (m_NextTexture == maxTextureCount)
        Flush();

    texture.Bind(m_NextTexture);

    m_NextVertex->Position = { position.x, position.y - size.y };
    m_NextVertex->Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
    m_NextVertex->TexCoord = { 0.0f, 0.0f };
    m_NextVertex->TextureSlot = static_cast<float>(m_NextTexture);
    m_NextVertex++;

    m_NextVertex->Position = { position.x + size.x, position.y - size.y };
    m_NextVertex->Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
    m_NextVertex->TexCoord = { 1.0f, 0.0f };
    m_NextVertex->TextureSlot = static_cast<float>(m_NextTexture);
    m_NextVertex++;

    m_NextVertex->Position = { position.x + size.x, position.y };
    m_NextVertex->Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
    m_NextVertex->TexCoord = { 1.0f, 1.0f };
    m_NextVertex->TextureSlot = static_cast<float>(m_NextTexture);
    m_NextVertex++;

    m_NextVertex->Position = { position.x, position.y };
    m_NextVertex->Colour = { 1.0f, 1.0f, 1.0f, 1.0f };
    m_NextVertex->TexCoord = { 0.0f, 1.0f };
    m_NextVertex->TextureSlot = static_cast<float>(m_NextTexture);
    m_NextVertex++;

    static constexpr uint32_t indicies[] = {
        0, 1, 2,
        2, 3, 0,

        4, 5, 6,
        6, 7, 4,
    };

    for (size_t i = 0; i < 6; i++) {
        *m_NextIndex = vertexCount + indicies[i];
        m_NextIndex++;
    }

    m_NextTexture++;
}

void Renderer::Flush() {
    m_VertexArray.Bind();
    m_IndexBuffer.Bind();
    //m_Shader.Bind();

    uint32_t vertexCount = m_NextVertex - m_Vertecies;
    uint32_t indexCount = m_NextIndex - m_Indicies;

    m_VertexBuffer.SetData(m_Vertecies, vertexCount * sizeof(Vertex));
    m_IndexBuffer.SetData(m_Indicies, indexCount);

    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);

    m_NextVertex = m_Vertecies;
    m_NextIndex = m_Indicies;
    m_NextTexture = 1;
}


void Renderer::Clear() {
    glClear(GL_COLOR_BUFFER_BIT);
}
