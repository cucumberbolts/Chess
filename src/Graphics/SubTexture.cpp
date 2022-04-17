#include "SubTexture.h"

SubTexture::SubTexture(Texture& texture, glm::vec2 position, glm::vec2 tileSize, glm::vec2 spriteSize) : m_Texture(&texture) {
	float width = tileSize.x * spriteSize.x / (float)texture.GetWidth();
	float height = tileSize.y * spriteSize.y / (float)texture.GetHeight();

	position = position * tileSize / texture.GetSize();

	m_TextureCoordinates[0] = position;
	m_TextureCoordinates[1] = { position.x + width, position.y };
	m_TextureCoordinates[2] = { position.x + width, position.y + height };
	m_TextureCoordinates[3] = { position.x, position.y + height };
}
