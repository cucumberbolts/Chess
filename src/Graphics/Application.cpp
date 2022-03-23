#include "Application.h"

#include <glad/glad.h>

#include <iostream>

Application* Application::s_Instance = nullptr;

Application::Application(uint32_t width, uint32_t height, const std::string& name) {
    if (!s_Instance)
        s_Instance = this;

    if (!glfwInit()) {
        std::cout << "Could not initialize GLFW!\n";
        return;
    }

    m_Window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    if (!m_Window) {
        std::cout << "Could not create window!\n";
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_Window);

    glfwSetWindowUserPointer(m_Window, this);

    glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
    {
        Application* app = (Application*)glfwGetWindowUserPointer(window);
        app->OnWindowClose();
    });

    glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        Application* app = (Application*)glfwGetWindowUserPointer(window);
        app->OnKeyPressed(key, scancode, action, mods);
    });

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}

Application::~Application() {
    glfwTerminate();
}

void Application::Run() {
    m_Running = true;

    // Temporary OpenGL code
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << "\n";

    uint32_t vertexArray;
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);

    float vertexBufferData[] = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f,  0.5f
    };

    uint32_t vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, false, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    uint32_t indexBufferData[] = {
        0, 1, 2,
        2, 3, 0
    };

    uint32_t indexBuffer;
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexBufferData), indexBufferData, GL_STATIC_DRAW);

    uint32_t program = glCreateProgram();

    std::string vertexShaderSource = R"(
#version 330 core

layout(location = 0) in vec4 position;

void main() {
    gl_Position = position;
}
    )";

    uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* shaderSource = vertexShaderSource.c_str();
    glShaderSource(vertexShader, 1, &shaderSource, nullptr);
    glCompileShader(vertexShader);

    std::string fragmentShaderSource = R"(
#version 330 core

layout(location = 0) out vec4 colour;

void main() {
    colour = vec4(0.0, 1.0, 1.0, 1.0);
}
    )";

    uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    shaderSource = fragmentShaderSource.c_str();
    glShaderSource(fragmentShader, 1, &shaderSource, nullptr);
    glCompileShader(fragmentShader);

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glValidateProgram(program);

    glUseProgram(program);

    while (m_Running) {
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(m_Window);

        glfwPollEvents();
    }
}

void Application::OnWindowClose() {
    m_Running = false;

    std::cout << "Closing window!\n";
}

void Application::OnKeyPressed(int32_t key, int32_t scancode, int32_t action, int32_t mods) {
    if (key == GLFW_KEY_ESCAPE) {
        m_Running = false;
    }
}
