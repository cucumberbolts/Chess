#include "Application.h"
#include "DebugContext.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <iostream>

Application* Application::s_Instance = nullptr;

static glm::vec4 HexToColour(uint32_t colour) {
    uint8_t r = 0, g = 0, b = 0, a = 0;
    return { r / 255.f, g / 255.f, b / 255.f, a / 255.f};
}

Application::Application(uint32_t width, uint32_t height, const std::string& name)
    : m_WindowProperties{ width, height, name } {
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

    glfwSwapInterval(1);

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

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    ImGui_ImplOpenGL3_Init("#version 450");
}

Application::~Application() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
}

void Application::Run() {
    m_Running = true;

    // Temporary OpenGL code
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << "\n";

    Shader shader("resources/shaders/VertexShader.glsl", "resources/shaders/FragmentShader.glsl");

    Texture texture("resources/textures/smiley.png", 6);

    glm::mat4 projectionMatrix = glm::ortho(-8.f, 8.f, -4.5f, 4.5f);
    glm::mat4 viewMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0));
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0));

    shader.SetUniform("u_MVP", projectionMatrix * viewMatrix * modelMatrix);

    int32_t textures[32];
    for (int i = 0; i < 32; i++)
        textures[i] = i;

    shader.SetUniform("u_Textures", textures, sizeof(textures));

    glm::vec3 rectPosition = { -2.0f, 0.5f, 0 };
    glm::vec4 rectColour = { 1.0f, 0.7f, 0.5, 1.0f };
    // #FF6600FF is a cool colour

    Renderer renderer;

    while (m_Running) {
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        renderer.Clear();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //ImGui::ShowDemoWindow();

        ImGui::Begin("Rectangle position");
        ImGui::SliderFloat("x position", &rectPosition.x, -8.f, 8.f);
        ImGui::SliderFloat("y position", &rectPosition.y, -4.5f, 4.5f);
        ImGui::End();

        ImGui::Begin("Rectangle colour");
        ImGui::ColorPicker4("Rectangle colour", &rectColour[0]);
        ImGui::End();

        renderer.DrawRect(rectPosition, { 1.0f, 1.0f }, rectColour);
        renderer.DrawRect({ 3.0f, 3.0f, 0}, { 1.0f, 1.0f }, texture);

        renderer.Flush();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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
