#include "Application.h"
#include "Buffer.h"
#include "DebugContext.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

Application* Application::s_Instance = nullptr;

Application::Application(uint32_t width, uint32_t height, const std::string& name) {
    if (!s_Instance)
        s_Instance = this;

    if (!glfwInit()) {
        std::cout << "Could not initialize GLFW!\n";
        return;
    }

#if _DEBUG
    glfwWindowHint(GLFW_CONTEXT_DEBUG, true);
#endif

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

    DebugContext::Init();
}

Application::~Application() {
    glfwTerminate();
}

void Application::Run() {
    m_Running = true;

    // Temporary OpenGL code
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << "\n";
    
    Vertex vertecies[] = {
        { -0.5f, -0.5f, 0.0f, 0.0f },
        {  0.5f, -0.5f, 1.0f, 0.0f },
        {  0.5f,  0.5f, 1.0f, 1.0f },
        { -0.5f,  0.5f, 0.0f, 1.0f }
    };

    VertexBuffer vertexBuffer(vertecies, 4);

    VertexArray vertexArray(vertexBuffer, {
            { VertexAttribute::Type::Float, 2 },
            { VertexAttribute::Type::Float, 2 },
        }
    );

    uint32_t indicies[] = {
        0, 1, 2,
        2, 3, 0
    };

    IndexBuffer indexBuffer(indicies, 6);

    Shader shader("resources/shaders/VertexShader.glsl", "resources/shaders/FragmentShader.glsl");
    //shader.SetUniform("u_Colour", 1.0f, 1.0f, 1.0f, 1.0f);

    Texture texture("resources/textures/smiley.png");
    texture.Bind(1);
    shader.SetUniform("u_Texture", 1);

    Renderer renderer;

    while (m_Running) {
        renderer.Clear();

        renderer.Draw(vertexArray, indexBuffer, shader, 6);

        glfwSwapBuffers(m_Window);

        glfwPollEvents();
    }
}

void Application::OnWindowClose() {
    m_Running = false;
}

void Application::OnKeyPressed(int32_t key, int32_t scancode, int32_t action, int32_t mods) {
    if (key == GLFW_KEY_ESCAPE) {
        m_Running = false;
    }
}
