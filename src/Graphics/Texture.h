#pragma once

#include <filesystem>

class Texture {
public:
    Texture(const std::filesystem::path& image, uint32_t slot);
    Texture(const Texture& other) = delete;
    ~Texture();

    inline int32_t GetWidth() const { return m_Width; }
    inline int32_t GetHeight() const { return m_Height; }
    inline int32_t GetBytesPerPixel() const { return m_BBP; }

    void Bind(uint32_t slot);
private:
    uint32_t m_TextureID = 0;

    int32_t m_Width = 0, m_Height = 0, m_BBP = 0;
};
