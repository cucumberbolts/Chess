#include "ChessApplication.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Chess/Board.h"
#include "ChessEngine/Engine.h"
#include "Graphics/Framebuffer.h"
#include "Graphics/Renderer.h"
#include "Graphics/SubTexture.h"
#include "Graphics/Texture.h"
#include "Utility/FileDialog.h"

#include "Resources.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

#include <iostream>
#include <sstream>

static glm::vec4 HexToColour(uint32_t colour) {
    static constexpr float toFloat = 1.0f / 255.0f;
    uint8_t r = (colour & 0xff000000) >> 24;
    uint8_t g = (colour & 0x00ff0000) >> 16;
    uint8_t b = (colour & 0x0000ff00) >> 8;
    uint8_t a = (colour & 0x000000ff);
    return glm::vec4{ r, g, b, a } * toFloat;
}

ChessApplication::ChessApplication(uint32_t width, uint32_t height, const std::string& name)
    : Application(width, height, name), m_ChessViewportSize(width, height) {
}

void ChessApplication::OnInit() {
    Renderer::Init(glm::ortho(-8.f, 8.f, -4.5f, 4.5f));

    std::cout << "OpenGL Version: " << Renderer::GetOpenGLVersion() << "\n";
    
    std::shared_ptr<Texture> chessPieces = std::make_shared<Texture>(Resources::Textures::CHESS_PIECES, sizeof(Resources::Textures::CHESS_PIECES));

    const float tileSize = (float)chessPieces->GetWidth() / 6.0f;
    for (int y = 0; y < 2; y++)
		for (int x = 0; x < 6; x++)
            m_ChessPieceSprites[y * 6 + x] = std::make_shared<SubTexture>(chessPieces, glm::vec2(x, y), glm::vec2(tileSize));

    m_DarkSquareColour = HexToColour(0x532A00FF);
    m_LightSquareColour = HexToColour(0xFFB160FF);
    m_LegalMoveColour = { 1.0f, 0.0f, 1.0f, 0.5f };
    m_BackgroundColour = { 0.2f, 0.2f, 0.2f, 1.0f };

    m_BoardFEN = Board::StartFen;
    m_BoardFEN.resize(100);

    FramebufferSpecification spec;
    spec.Width = m_WindowProperties.Width;
    spec.Height = m_WindowProperties.Height;
    m_ChessViewport = std::make_shared<Framebuffer>(spec);
}

void ChessApplication::OnRender() {
    Renderer::ClearScreen({ 0.0f, 0.0f, 0.0f, 1.0f });
    
    // Resize framebuffer according to viewport size
    FramebufferSpecification spec = m_ChessViewport->GetSpecification();
    if ((float)spec.Width != m_ChessViewportSize.x || (float)spec.Height != m_ChessViewportSize.y) {
        m_ChessViewport->Resize((uint32_t)m_ChessViewportSize.x, (uint32_t)m_ChessViewportSize.y);

        // I don't know what to call this variable
        constexpr float temp = 4.5f;

        float aspectRatio = m_ChessViewportSize.x / m_ChessViewportSize.y;

        // Resize the coordinates so the chessboard size
        // is the minimumm of the width and the height
        if (aspectRatio <= 1.0f)
            Renderer::UpdateProjectionMatrix(glm::ortho(-temp, temp, -temp / aspectRatio, temp / aspectRatio));
        else
            Renderer::UpdateProjectionMatrix(glm::ortho(-temp * aspectRatio, temp * aspectRatio, -temp, temp));
    }

    // Render chessboard to framebuffer
    m_ChessViewport->Bind();
    Renderer::ClearScreen(m_BackgroundColour);

    // Draw chess board
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            const glm::vec4 colour = (x + y) % 2 == 0 ? m_LightSquareColour : m_DarkSquareColour;
            Renderer::DrawRect({ -3.5f + x, 3.5f - y, -0.5f }, { 1.0f, 1.0f }, colour);
        }
    }

    // Draw legal moves (loop through m_LegalMoves bitboard)
    for (BitBoard legalMoves = m_LegalMoves; legalMoves; legalMoves &= legalMoves - 1) {
        Square square = GetSquare(legalMoves);
        int rank = RankOf(square);
        int file = FileOf(square);
        Renderer::DrawRect({ -3.5f + file, -3.5f + rank, 0.0f }, { 1.0f, 1.0f }, m_LegalMoveColour);
    }

    // Draw pieces
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            std::shared_ptr<SubTexture> piece;
            switch (m_Board[y * 8 + x]) {
                case WhitePawn:     piece = m_ChessPieceSprites[11]; break;
                case WhiteKnight:   piece = m_ChessPieceSprites[9];  break;
                case WhiteBishop:   piece = m_ChessPieceSprites[8];  break;
                case WhiteRook:     piece = m_ChessPieceSprites[10]; break;
                case WhiteQueen:    piece = m_ChessPieceSprites[7];  break;
                case WhiteKing:     piece = m_ChessPieceSprites[6];  break;
                case BlackPawn:     piece = m_ChessPieceSprites[5];  break;
                case BlackKnight:   piece = m_ChessPieceSprites[3];  break;
                case BlackBishop:   piece = m_ChessPieceSprites[2];  break;
                case BlackRook:     piece = m_ChessPieceSprites[4];  break;
                case BlackQueen:    piece = m_ChessPieceSprites[1];  break;
                case BlackKing:     piece = m_ChessPieceSprites[0];  break;
                case None: continue;
            }

            if (m_SelectedPiece == y * 8 + x && m_IsHoldingPiece) {
                // Draw selected piece following the mouse
                glm::mat4 transform = glm::inverse(Renderer::GetProjectionMatrix());
                transform = glm::scale(transform, { 1 / (m_ChessViewportSize.x * 0.5f), 1 / (m_ChessViewportSize.y * -0.5f), 1.0f });
                transform = glm::translate(transform, { m_ChessViewportSize.x * -0.5f, m_ChessViewportSize.y * -0.5f, 0.0f });
                glm::vec4 point = transform * glm::vec4{ m_MousePosition, 1.0f, 1.0f };

                Renderer::DrawRect({ point.x, point.y, 0.5f }, { 1, 1 }, piece);
            }
            else {
                Renderer::DrawRect({ -3.5f + x, -3.5f + y, 0.0f }, { 1.0f, 1.0f }, piece);
            }
        }
    }

    Renderer::Flush();

    // Render ImGui to window
    m_ChessViewport->Unbind();
}

void ChessApplication::RenderImGui() {
    {
        // Fullscreen stuff
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
        window_flags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", nullptr, window_flags);
        ImGui::PopStyleVar();

        ImGui::PopStyleVar(2);

        // Submit the DockSpace
        static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;
        ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                ImGui::MenuItem("thing");
                ImGui::Separator();

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        ImGui::End();
    }

    {
        ImGui::Begin("Colours");
        ImGui::ColorPicker4("Dark square colour", glm::value_ptr(m_DarkSquareColour));
        ImGui::ColorPicker4("Light square colour", glm::value_ptr(m_LightSquareColour));
        ImGui::ColorPicker4("Legal move colour", glm::value_ptr(m_LegalMoveColour));
        ImGui::ColorPicker4("Background colour", glm::value_ptr(m_BackgroundColour));
        ImGui::End();
    }

    {
        ImGui::Begin("FEN: ");
        
        if (ImGui::InputText("FEN", m_BoardFEN.data(), m_BoardFEN.size(), ImGuiInputTextFlags_EnterReturnsTrue))
            m_Board.FromFEN(m_BoardFEN);

        if (ImGui::Button("Copy FEN to clipboard"))
            glfwSetClipboardString(m_Window, m_BoardFEN.c_str());

        if (ImGui::Button("Reset board")) {
            m_Board.Reset();  // Reset FEN string
            m_BoardFEN = Board::StartFen;
        }

        ImGui::End();
    }
    
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
        //ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, { 800.f, 800.f });  // Doesn't work in dockspace

        ImGui::Begin("Chessboard");

        // Get the position of the viewport relative to the panel position
        // (The tab bar pushes it down)
        auto [offsetX, offsetY] = ImGui::GetCursorPos();

        ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        m_ChessViewportSize = { viewportSize.x, viewportSize.y };

        ImTextureID texture = (void*)m_ChessViewport->GetColourAttachment();
        int framePadding = 0;
        ImVec4 backgroundColour = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
        ImVec4 tintColour = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        // Using ImageButton allows the viewport to not be dragged when it is floating
        ImGui::ImageButton(texture, viewportSize, { 0, 1 }, { 1, 0 }, framePadding, backgroundColour, tintColour);

        // Get mouse position on the board
        auto [mouseX, mouseY] = ImGui::GetMousePos();
        auto [windowX, windowY] = ImGui::GetWindowPos();

        glm::vec2 mousePosition = { mouseX, mouseY };
        glm::vec2 viewportPosition = { windowX + offsetX, windowY + offsetY };

        m_MousePosition = mousePosition - viewportPosition;

        ImGui::End();

        ImGui::PopStyleVar();
    }

    {
        ImGui::Begin("Engine");

        static auto s_SelectedEngine = m_Engines.end();

        if (s_SelectedEngine == m_Engines.end()) {
            if (ImGui::Button("Create engine")) {
                ImGui::OpenPopup("Create engine");
            }

#define GetCentre GetCenter

            // Always centre this window when appearing
            ImVec2 centre = ImGui::GetMainViewport()->GetCentre();
            ImGui::SetNextWindowPos(centre, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

            if (ImGui::BeginPopupModal("Create engine", nullptr, ImGuiPopupFlags_None)) {
                static char path[255] = { 0 };
                static char name[64] = { 0 };

                ImGui::InputText("path", path, sizeof(path) / sizeof(char));
                if (ImGui::Button("Select executable")) {
                    FileDialog::Open(path, sizeof(path));

                    // If the 'name' field is empty, copy the executable name to it
                    if (name[0] == '\0')
                        strcpy_s(name, sizeof(name), std::filesystem::path(path).stem().u8string().c_str());
                }
                
                ImGui::InputText("name", name, sizeof(name) / sizeof(char));

                if (ImGui::Button("OK", ImVec2(120, 0))) {
                    m_Engines.emplace_back(std::pair{ name, path });
                    s_SelectedEngine = m_Engines.end();

                    // "Reset" the strings for next time popup is opened
                    path[0] = '\0';
                    name[0] = '\0';
                    ImGui::CloseCurrentPopup();
                }

                ImGui::SameLine();

                if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                    // "Reset" the strings for next time popup is opened
                    path[0] = '\0';
                    name[0] = '\0';
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }

            for (auto it = m_Engines.begin(); it != m_Engines.end(); ++it) {
                const std::string& name = it->first;
                const std::filesystem::path& path = it->second;

                ImGui::Separator();

                ImGui::Text("%s", name.c_str());

                ImGui::PushID(std::distance(m_Engines.begin(), it));

                if (ImGui::Button("Start engine")) {
                    m_RunningEngine = Engine::Create(path);
                    m_RunningEngine->SetUpdateCallback([this](const Engine::BestContinuation& c) { OnEngineUpdate(c); });
                    m_RunningEngine->Init();
                    m_RunningEngine->SetPosition(m_BoardFEN);
                    m_RunningEngine->Run();
                    s_SelectedEngine = it;
                }

                ImGui::PopID();
            }
        } else {
            ImGui::Text("%s", s_SelectedEngine->first.c_str());

            if (ImGui::Button("Stop engine")) {
                m_RunningEngine->Stop();  // Stop the engine
                m_RunningEngine.reset();  // Kill the process
                s_SelectedEngine = m_Engines.end();
            } else {                
                ImGui::Text("%s", m_BestContinuationAlgebraicMoves.c_str());
                if (ImGui::Button("Copy to clipboard"))
                    ImGui::SetClipboardText(m_BestContinuationAlgebraicMoves.c_str());
            }
        }

        ImGui::End();
    }
}

void ChessApplication::OnWindowClose() {
    m_Running = false;
}

void ChessApplication::OnWindowResize(int32_t width, int32_t height) {
    m_WindowProperties.Width = width;
    m_WindowProperties.Height = height;
}

void ChessApplication::OnKeyPressed(int32_t key, int32_t scancode, int32_t action, int32_t mods) {
    if (key == GLFW_KEY_ESCAPE) {
        m_Running = false;
    }
}

void ChessApplication::OnMouseButton(int32_t button, int32_t action, int32_t mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
#if 0
        float x = (m_MousePosition.x - m_ChessViewportSize.x * 0.5f) / (m_ChessViewportSize.x * 0.5f);
        float y = (m_MousePosition.y - m_ChessViewportSize.y * 0.5f) / (m_ChessViewportSize.y * -0.5f);
        
        glm::vec4 point = { x, y, 1.0f, 1.0f };
        point = glm::inverse(Renderer::GetProjectionMatrix()) * point;

        x = point.x;
        y = point.y;
#elif 0
        glm::vec4 point = { m_MousePosition, 1.0f, 1.0f };
        point = glm::translate(glm::mat4(1.0f), { m_ChessViewportSize.x * -0.5f, m_ChessViewportSize.y * -0.5f, 0.0f }) * point;
        point = glm::scale(glm::mat4(1.0f), { 1 / (m_ChessViewportSize.x * 0.5f), 1 / (m_ChessViewportSize.y * 0.5f), 1.0f }) * point;
        point = glm::inverse(Renderer::GetProjectionMatrix()) * point;

        float x = point.x;
        float y = point.y;
#else
        // Convert ImGui viewport coordinates to rendering coordinates
        // TODO: Construct matrix by hand??
        // TODO: Only calculate once
        glm::mat4 transform = glm::inverse(Renderer::GetProjectionMatrix());
        transform = glm::scale(transform, { 1 / (m_ChessViewportSize.x * 0.5f), 1 / (m_ChessViewportSize.y * -0.5f), 1.0f });
        transform = glm::translate(transform, { m_ChessViewportSize.x * -0.5f, m_ChessViewportSize.y * -0.5f, 0.0f });
        glm::vec4 point = transform * glm::vec4{ m_MousePosition, 1.0f, 1.0f };

        float& x = point.x;
        float& y = point.y;
#endif

        if (action == GLFW_PRESS) {
            m_IsHoldingPiece = true;

            if (x > -4 && x < 4 && y > -4 && y < 4) {
                Square rank = (Square)(x + 4.0f);
                Square file = (Square)(y + 4.0f);

                // The square the mouse clicked on
                Square selectedSquare = ToSquare('a' + rank, '1' + file);

                // If a piece was already selected, move piece to clicked square
                if (m_SelectedPiece != INVALID_SQUARE && m_SelectedPiece != selectedSquare) {
                    if (m_LegalMoves & (1ull << selectedSquare) || selectedSquare == m_SelectedPiece) {
                        m_Board.Move({ m_SelectedPiece, selectedSquare });
                        m_BoardFEN = m_Board.ToFEN();
                        if (m_RunningEngine)
                            m_RunningEngine->SetPosition(m_BoardFEN);
                    }

                    m_SelectedPiece = INVALID_SQUARE;
                    m_LegalMoves = 0;
                }
                else {  // If no piece already selected, select piece
                    m_LegalMoves = m_Board.GetPieceLegalMoves(selectedSquare);
                    m_SelectedPiece = m_LegalMoves == 0 ? INVALID_SQUARE : selectedSquare;
                }
            }
            else {
                m_SelectedPiece = INVALID_SQUARE;
                m_LegalMoves = 0;
            }
        }
        else if (action == GLFW_RELEASE) {
            if (x > -4 && x < 4 && y > -4 && y < 4) {
                Square rank = (Square)(x + 4.0f);
                Square file = (Square)(y + 4.0f);

                // The square the mouse was released on
                Square selectedSquare = ToSquare('a' + rank, '1' + file);

                if (m_SelectedPiece != INVALID_SQUARE) {
                    if (m_LegalMoves & (1ull << selectedSquare)) {
                        m_Board.Move({ m_SelectedPiece, selectedSquare });
                        m_BoardFEN = m_Board.ToFEN();
                        if (m_RunningEngine)
							m_RunningEngine->SetPosition(m_BoardFEN);
                        m_SelectedPiece = INVALID_SQUARE;
                        m_LegalMoves = 0;
                    }
                }
            }

            m_IsHoldingPiece = false;
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        m_SelectedPiece = INVALID_SQUARE;
        m_IsHoldingPiece = false;
        m_LegalMoves = 0;
    }
}

void ChessApplication::OnEngineUpdate(const Engine::BestContinuation& bestContinuation) {
    m_BestContinuation = bestContinuation;
    
    Board moveTranslator(m_Board);

    std::ostringstream continuationText;
    for (LongAlgebraicMove m : m_BestContinuation.Continuation)
        continuationText << moveTranslator.Move(m) << " ";

    m_BestContinuationAlgebraicMoves = continuationText.str();
}
