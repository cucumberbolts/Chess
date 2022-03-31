#pragma once

#include "Buffer.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"

#include <glm/glm.hpp>

// TODO: Make singleton?
class Renderer {
public:
    Renderer();
    ~Renderer();

    void DrawRect(glm::vec3 position, glm::vec2 size, glm::vec4 colour);
    void DrawRect(glm::vec3 position, glm::vec2 size, Texture& texture);

    void Flush();

    void Clear();
private:
    //Shader m_Shader;
    IndexBuffer m_IndexBuffer;
    VertexBuffer m_VertexBuffer;
    VertexArray m_VertexArray;

    Vertex* m_Vertecies = nullptr;
    Vertex* m_NextVertex = nullptr;
    uint32_t* m_Indicies = nullptr;
    uint32_t* m_NextIndex = nullptr;

    Texture m_WhiteTexture;
    uint32_t m_NextTexture = 1;  // Slot of next texture. Begins at 1 because 0 is for the white texture
};
