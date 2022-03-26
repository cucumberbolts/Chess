#include "Shader.h"

#include <fstream>

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

Shader::~Shader() {
    glDeleteProgram(m_ShaderID);
}

void Shader::Bind() const {
    glUseProgram(m_ShaderID);
}
