#include "Shader.h"

#include <fstream>
#include <iostream>

#include <glad/glad.h>

static uint32_t CompileShader(GLenum type, const char* source) {
    uint32_t shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    return shader;
}

Shader::Shader(const std::filesystem::path& vertexShader, const std::filesystem::path& fragmentShader) {
    m_ShaderID = glCreateProgram();

    std::fstream input(vertexShader);
    std::string vertexSource(std::filesystem::file_size(vertexShader), ' ');
    input.read(vertexSource.data(), std::filesystem::file_size(vertexShader));
    input.close();

    input.open(fragmentShader);
    std::string fragmentSource(std::filesystem::file_size(vertexShader), ' ');
    input.read(fragmentSource.data(), std::filesystem::file_size(vertexShader));
    input.close();

    uint32_t vs = CompileShader(GL_VERTEX_SHADER, vertexSource.c_str());
    uint32_t fs = CompileShader(GL_FRAGMENT_SHADER, fragmentSource.c_str());

    glAttachShader(m_ShaderID, vs);
    glAttachShader(m_ShaderID, fs);
    glLinkProgram(m_ShaderID);
    glValidateProgram(m_ShaderID);

    glDetachShader(m_ShaderID, vs);
    glDetachShader(m_ShaderID, fs);

    glUseProgram(m_ShaderID);
}

Shader::Shader(const char* vertexShader, const char* fragmentShader) {
    m_ShaderID = glCreateProgram();

    uint32_t vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    uint32_t fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(m_ShaderID, vs);
    glAttachShader(m_ShaderID, fs);
    glLinkProgram(m_ShaderID);
    glValidateProgram(m_ShaderID);

    glDetachShader(m_ShaderID, vs);
    glDetachShader(m_ShaderID, fs);

    glUseProgram(m_ShaderID);
}

Shader::~Shader() {
    glDeleteProgram(m_ShaderID);
}

void Shader::Bind() const {
    glUseProgram(m_ShaderID);
}

void Shader::SetUniform(const std::string& name, float a, float b, float c, float d) {
    int32_t location = GetUniformLocation(name);
    if (location != -1)
        glUniform4f(location, a, b, c, d);
}

void Shader::SetUniform(const std::string& name, int32_t a) {
    int32_t location = GetUniformLocation(name);
    if (location != -1)
        glUniform1i(location, a);
}

void Shader::SetUniform(const std::string& name, int32_t* a, uint32_t count) {
    int32_t location = GetUniformLocation(name);
    if (location != -1)
        glUniform1iv(location, count, a);
}

void Shader::SetUniform(const std::string& name, const glm::mat4& a) {
    int32_t location = GetUniformLocation(name);
    if (location != -1)
        glUniformMatrix4fv(location, 1, GL_FALSE, &a[0][0]);
}

int32_t Shader::GetUniformLocation(const std::string& name) const {
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
        return m_UniformLocationCache[name];
    
    m_UniformLocationCache[name] = glGetUniformLocation(m_ShaderID, name.c_str());
    return m_UniformLocationCache[name];
}
