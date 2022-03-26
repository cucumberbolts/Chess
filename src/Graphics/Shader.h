#pragma once

#include <filesystem>
#include <unordered_map>

class Shader {
public:
    Shader(const std::filesystem::path& vertexShader, const std::filesystem::path& fragmentShader);
    ~Shader();

    void Bind() const;

    void SetUniform(const std::string& name, float a, float b, float c, float d);
    void SetUniform(const std::string& name, int32_t a);
private:
    int32_t GetUniformLocation(const std::string& name) const;
private:
    uint32_t m_ShaderID = 0;

    mutable std::unordered_map<std::string, int32_t> m_UniformLocationCache;
};
