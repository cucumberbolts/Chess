#pragma once

#include "SubTexture.h"
#include "Texture.h"

#include <glm/glm.hpp>

class Renderer {
public:
    static void Init(glm::mat4 projection);

    Renderer() = delete;
    Renderer(const Renderer& other) = delete;
    Renderer(Renderer&& other) = delete;
    ~Renderer();

    Renderer& operator=(const Renderer& other) = delete;
    Renderer& operator=(Renderer&& other) = delete;

    static void DrawRect(glm::vec3 position, glm::vec2 size, glm::vec4 colour);
    static void DrawRect(glm::vec3 position, glm::vec2 size, const Texture& texture);
    static void DrawRect(glm::vec3 position, glm::vec2 size, const SubTexture& subTexture);

    static void Flush();

    static void SetClearColour(glm::vec4 colour);
    static void ClearScreen();
};
