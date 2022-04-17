#pragma once

#include "Texture.h"

#include <glm/glm.hpp>

class SubTexture {
public:
	SubTexture() = default;
	SubTexture(const std::shared_ptr<Texture>& texture, glm::vec2 position, glm::vec2 tileSize, glm::vec2 spriteSize = { 1.0f, 1.0f });

	const std::shared_ptr<Texture>& GetTexture() const { return m_Texture; }
	const glm::vec2* GetTextureCoordinates() const { return m_TextureCoordinates; }
private:
	std::shared_ptr<Texture> m_Texture;

	glm::vec2 m_TextureCoordinates[4] = { { 0, 0 } };
};
