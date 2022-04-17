#pragma once

#include <filesystem>

#include <glm/glm.hpp>

class Texture {
public:
    Texture(int32_t width, int32_t height);
    Texture(uint8_t* data, int32_t width, int32_t height);
    Texture(const std::filesystem::path& image);
    Texture(const Texture&) = delete;
    ~Texture();

    Texture& operator=(const Texture&) = delete;

    bool operator==(const Texture& other) const { return m_TextureID == other.m_TextureID; }

    uint32_t GetSlot() const { return m_Slot; }
    int32_t GetWidth() const { return m_Width; }
    int32_t GetHeight() const { return m_Height; }
    int32_t GetBytesPerPixel() const { return m_BBP; }
    glm::vec2 GetSize() const { return { m_Width, m_Height }; }

    void Bind(uint32_t slot);

    void SetData(void* data);
private:
    uint32_t m_TextureID = 0;

    uint32_t m_Slot = 0;

    int32_t m_Width = 0, m_Height = 0, m_BBP = 0;
};
