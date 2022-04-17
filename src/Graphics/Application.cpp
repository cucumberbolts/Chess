#include "Application.h"
#include "Chess/Board.h"
#include "DebugContext.h"
#include "Renderer.h"
#include "SubTexture.h"
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
    uint8_t r = (colour & 0xff000000) >> 24;
	uint8_t g = (colour & 0x00ff0000) >> 16;
    uint8_t b = (colour & 0x0000ff00) >> 8;
	uint8_t a = (colour & 0x000000ff);
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

    Renderer::Init(glm::ortho(-8.f, 8.f, -4.5f, 4.5f));
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

    Texture texture("resources/textures/smiley.png");

    Board board;

    Texture chessPieces("resources/textures/Chess_Pieces_Sprite.png");
    SubTexture chessPieceSprites[12];
    for (int x = 0; x < 6; x++)
        for (int y = 0; y < 2; y++)
            chessPieceSprites[y * 6 + x] = SubTexture(chessPieces, { x, y }, { 45.0f, 45.0f });

    glm::vec4 darkColour = HexToColour(0x532A00FF);
    glm::vec4 lightColour = HexToColour(0xFFB160FF);

    Renderer::SetClearColour({ 0.2f, 0.2f, 0.2f, 1.0f });

    while (m_Running) {
        Renderer::ClearScreen();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //ImGui::ShowDemoWindow();
        
        ImGui::Begin("Square colours");
        ImGui::ColorPicker4("Dark square colour", &darkColour[0]);
        ImGui::ColorPicker4("Light square colour", &lightColour[0]);
        ImGui::End();

        for (int y = 0; y < 8; y++) {
			for (int x = 0; x < 8; x++) {
                const glm::vec4 colour = (x + y) % 2 == 0 ? lightColour : darkColour;
                Renderer::DrawRect({ -4.0f + x, 4.0f - y, 0.0f }, { 1, 1 }, colour);
            }
        }
        
        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                SubTexture piece;
                switch (board[y * 8 + x]) {
					case WhitePawn:     piece = chessPieceSprites[11]; break;
	                case WhiteKnight:   piece = chessPieceSprites[9];  break;
	                case WhiteBishop:   piece = chessPieceSprites[8];  break;
	                case WhiteRook:     piece = chessPieceSprites[10]; break;
	                case WhiteQueen:    piece = chessPieceSprites[7];  break;
	                case WhiteKing:     piece = chessPieceSprites[6];  break;
	                case BlackPawn:     piece = chessPieceSprites[5];  break;
	                case BlackKnight:   piece = chessPieceSprites[3];  break;
	                case BlackBishop:   piece = chessPieceSprites[2];  break;
	                case BlackRook:     piece = chessPieceSprites[4];  break;
	                case BlackQueen:    piece = chessPieceSprites[1];  break;
	                case BlackKing:     piece = chessPieceSprites[0];  break;
	                case None: continue;
                }
        
                Renderer::DrawRect({ -4.0f + x, 4.0f - y, 0.0f }, { 1, 1 }, piece);
            }
        }

        Renderer::Flush();

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
