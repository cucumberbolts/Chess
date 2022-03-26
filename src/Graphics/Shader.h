#pragma once

#include <filesystem>

class Shader {
public:
    Shader(const std::filesystem::path& vertexShader, const std::filesystem::path& fragmentShader);
    ~Shader();

    void Bind() const;
private:
    uint32_t m_ShaderID = 0;
};
