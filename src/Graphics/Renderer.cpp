#include "Buffer.h"
#include "Renderer.h"

#include <array>

#include "Shader.h"
#include "VertexArray.h"

#include <memory>
#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>

namespace {

    constexpr size_t maxQuadCount = 100;
    constexpr size_t maxVertexCount = maxQuadCount * 4;
    constexpr size_t maxIndexCount = maxQuadCount * 6;

    constexpr int32_t maxTextureCount = 32;

    struct Vertex {
        glm::vec4 Position;
        glm::vec4 Colour;
        glm::vec2 TexCoord;
        float TextureSlot;
    };

    constexpr std::initializer_list<VertexAttribute> s_VertexLayout = {
        { VertexAttribute::Type::Float, 4 },  // Position
        { VertexAttribute::Type::Float, 4 },  // Colour
        { VertexAttribute::Type::Float, 2 },  // Texture coordinates
        { VertexAttribute::Type::Float, 1 },  // Texture ID
    };

    std::unique_ptr<IndexBuffer> s_IndexBuffer;
    std::unique_ptr<Shader> s_Shader;
    std::unique_ptr<VertexBuffer> s_VertexBuffer;
    std::unique_ptr<VertexArray> s_VertexArray;

    Vertex* s_Vertecies = nullptr;
    Vertex* s_NextVertex = nullptr;
    uint32_t* s_Indicies = nullptr;
    uint32_t* s_NextIndex = nullptr;

    std::array<std::shared_ptr<Texture>, maxTextureCount> m_Textures;
    uint32_t m_NextTexture = 1;  // Slot of next texture. Begins at 1 because 0 is for the white texture

    constexpr std::array<glm::vec4, 4> s_RectPositions = {
        glm::vec4{ -0.5f, -0.5f, 0.0f, 1.0f },
        glm::vec4{  0.5f, -0.5f, 0.0f, 1.0f },
        glm::vec4{  0.5f,  0.5f, 0.0f, 1.0f },
        glm::vec4{ -0.5f,  0.5f, 0.0f, 1.0f },
    };

    constexpr std::array<uint32_t, 12> s_RectIndicies = {
        0, 1, 2,
        2, 3, 0,

        4, 5, 6,
        6, 7, 4,
    };

    constexpr std::array<glm::vec2, 4> s_TextureCoords = {
        glm::vec2{ 0.0f, 0.0f },
        glm::vec2{ 1.0f, 0.0f },
        glm::vec2{ 1.0f, 1.0f },
        glm::vec2{ 0.0f, 1.0f },
    };

}

void Renderer::Init(const glm::mat4& projection) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    s_IndexBuffer = std::make_unique<IndexBuffer>(maxIndexCount * sizeof(uint32_t));
    s_Shader = std::make_unique<Shader>("resources/shaders/VertexShader.glsl", "resources/shaders/FragmentShader.glsl");
    s_VertexBuffer = std::make_unique<VertexBuffer>(maxVertexCount * sizeof(Vertex));
    s_VertexArray = std::make_unique<VertexArray>(*s_VertexBuffer, s_VertexLayout);

    constexpr uint32_t whitePixel = 0xFFFFFFFF;
    m_Textures[0] = std::make_shared<Texture>((uint8_t*)&whitePixel, 1, 1);
    m_Textures[0]->Bind(0.0f);

    s_Vertecies = new Vertex[maxVertexCount];
    s_Indicies = new uint32_t[maxIndexCount];

    s_NextVertex = s_Vertecies;
    s_NextIndex = s_Indicies;

    glm::mat4 projectionMatrix = projection;
    glm::mat4 viewMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0));
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0));

    s_Shader->SetUniform("u_MVP", projectionMatrix * viewMatrix * modelMatrix);

    int32_t textures[maxTextureCount];

    for (int32_t i = 0; i < maxTextureCount; i++)
        textures[i] = i;

    s_Shader->SetUniform("u_Textures", textures, sizeof(textures));
}

Renderer::~Renderer() {
    delete[] s_Vertecies;
    delete[] s_Indicies;
}

void Renderer::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& colour) {
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
		* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

    DrawRect(transform, colour, m_Textures[0], s_TextureCoords.data());
}

void Renderer::DrawRect(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<Texture>& texture) {
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
        * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

    DrawRect(transform, { 1.0f, 1.0f, 1.0f, 1.0f }, texture, s_TextureCoords.data());
}

void Renderer::DrawRect(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<SubTexture>& subTexture) {
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
        * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

    DrawRect(transform, { 1.0f, 1.0f, 1.0f, 1.0f }, subTexture->GetTexture(), subTexture->GetTextureCoordinates());
}

void Renderer::DrawRect(const glm::mat4& transform, const glm::vec4& colour, const std::shared_ptr<Texture>& texture, const glm::vec2* textureCoords) {
    uint64_t vertexCount = s_NextVertex - s_Vertecies;

    if (vertexCount > maxVertexCount - 4 || m_NextTexture == maxTextureCount) {
        Flush();
        vertexCount = s_NextVertex - s_Vertecies;
    }

    float textureSlot;

    const auto existingTexture = std::find(m_Textures.begin(), m_Textures.end(), texture);
    if (existingTexture == m_Textures.end()) {
        m_Textures[m_NextTexture] = texture;
        m_Textures[m_NextTexture]->Bind(m_NextTexture);
        textureSlot = (float)m_NextTexture;
        m_NextTexture++;
    } else {
        textureSlot = (float)(*existingTexture)->GetSlot();
    }

    //texture->Bind(m_NextTexture);
    //m_NextTexture++;
    //m_Textures[m_NextTexture] = texture;

    for (size_t i = 0; i < 4; i++) {
        s_NextVertex->Position = transform * s_RectPositions[i];
        s_NextVertex->Colour = colour;
        s_NextVertex->TexCoord = textureCoords[i];
        s_NextVertex->TextureSlot = textureSlot;
        s_NextVertex++;
    }

    for (size_t i = 0; i < 6; i++) {
        *s_NextIndex = vertexCount + s_RectIndicies[i];
        s_NextIndex++;
    }
}

void Renderer::Flush() {
    s_VertexArray->Bind();
    s_IndexBuffer->Bind();
    s_Shader->Bind();

    uint32_t vertexCount = s_NextVertex - s_Vertecies;
    uint32_t indexCount = s_NextIndex - s_Indicies;
    
    s_VertexBuffer->SetData(s_Vertecies, vertexCount * sizeof(Vertex));
    s_IndexBuffer->SetData(s_Indicies, indexCount);

    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);

    s_NextVertex = s_Vertecies;
    s_NextIndex = s_Indicies;
    m_NextTexture = 1;
}

void Renderer::ClearScreen(glm::vec4 colour) {
    glClearColor(colour.r, colour.g, colour.b, colour.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
