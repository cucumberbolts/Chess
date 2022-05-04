#pragma once

#include "SubTexture.h"
#include "Texture.h"

#include <glm/glm.hpp>

class Renderer {
public:
    static void Init(const glm::mat4& projection);

    Renderer() = delete;
    Renderer(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    ~Renderer();

    Renderer& operator=(const Renderer&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    static void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& colour);
    static void DrawRect(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<Texture>& texture);
    static void DrawRect(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<SubTexture>& subTexture);

    static void DrawRect(const glm::mat4& transform, const glm::vec4& colour, const std::shared_ptr<Texture>& texture, const glm::vec2* textureCoords);

    static void Flush();

    static void SetClearColour(glm::vec4 colour);
    static void ClearScreen();
};
