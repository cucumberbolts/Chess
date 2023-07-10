#include "ChessApplication.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Chess/Board.h"
#include "Graphics/Framebuffer.h"
#include "Graphics/Renderer.h"
#include "Graphics/SubTexture.h"
#include "Graphics/Texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

#include <iostream>

static glm::vec4 HexToColour(uint32_t colour) {
    uint8_t r = (colour & 0xff000000) >> 24;
    uint8_t g = (colour & 0x00ff0000) >> 16;
    uint8_t b = (colour & 0x0000ff00) >> 8;
    uint8_t a = (colour & 0x000000ff);
    return { r / 255.f, g / 255.f, b / 255.f, a / 255.f };
}

ChessApplication::ChessApplication(uint32_t width, uint32_t height, const std::string& name)
    : Application(width, height, name), m_ChessViewportSize(width, height) {
}

void ChessApplication::OnInit() {
    Renderer::Init(glm::ortho(-8.f, 8.f, -4.5f, 4.5f));

    std::cout << "OpenGL Version: " << Renderer::GetOpenGLVersion() << "\n";

    std::shared_ptr<Texture> chessPieces = std::make_shared<Texture>("resources/textures/Chess_Pieces_Sprite.png");
    for (int x = 0; x < 6; x++)
        for (int y = 0; y < 2; y++)
            m_ChessPieceSprites[y * 6 + x] = std::make_shared<SubTexture>(chessPieces, glm::vec2(x, y), glm::vec2(45.0f, 45.0f));

    m_DarkSquareColour = HexToColour(0x532A00FF);
    m_LightSquareColour = HexToColour(0xFFB160FF);
    m_LegalMoveColour = { 1.0f, 0.0f, 1.0f, 0.5f };

    m_BoardFEN = std::string(100, '\0');
    strcpy(m_BoardFEN.data(), Board::StartFen.data());

    FramebufferSpecification spec;
    spec.Width = m_WindowProperties.Width;
    spec.Height = m_WindowProperties.Height;
    m_ChessViewport = std::make_shared<Framebuffer>(spec);
}


void ChessApplication::OnRender() {
    Renderer::ClearScreen({ 0.0f, 1.0f, 0.0f, 1.0f });

    // Render chessboard to framebuffer
    m_ChessViewport->Bind();
    Renderer::ClearScreen({ 0.2f, 0.2f, 0.2f, 1.0f });

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
        int rank = RankOf(square) / 8;
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

                double x, y;
                glfwGetCursorPos(m_Window, &x, &y);

                x = (x - m_WindowProperties.Width / 2) / (m_WindowProperties.Width / 2) * 8.0;
                y = (y - m_WindowProperties.Height / 2) / (m_WindowProperties.Height / 2) * -4.5;

                Renderer::DrawRect({ x, y, 0.5f }, { 1, 1 }, piece);
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
        ImGui::Begin("Square colours");
        ImGui::ColorPicker4("Dark square colour", glm::value_ptr(m_DarkSquareColour));
        ImGui::ColorPicker4("Light square colour", glm::value_ptr(m_LightSquareColour));
        ImGui::End();
    }

    {
        ImGui::Begin("FEN: ");

        // Last part of FEN not showing...
        if (ImGui::InputText("FEN", m_BoardFEN.data(), m_BoardFEN.capacity() + 1, ImGuiInputTextFlags_EnterReturnsTrue))
            m_Board.FromFEN(m_BoardFEN);

        if (ImGui::Button("Copy FEN to clipboard"))
            glfwSetClipboardString(m_Window, m_BoardFEN.c_str());

        if (ImGui::Button("Reset board")) {
            m_Board.Reset();  // Reset FEN string
            m_BoardFEN = Board::StartFen;
        }

        ImGui::End();
    }

    // TODO: 1. Resizing the chessboard viewport (and maintain aspect ratio)
    // TODO: 2. Update mouse picking
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
        ImGui::Begin("Chessboard");

        ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        if (m_ChessViewportSize.x != viewportSize.x || m_ChessViewportSize.y != viewportSize.y) {
            m_ChessViewportSize = { viewportSize.x, viewportSize.y };
            m_ChessViewport->Resize((uint32_t)m_ChessViewportSize.x, (uint32_t)m_ChessViewportSize.y);
        }

        ImGui::Image((void*)m_ChessViewport->GetColourAttachment(), viewportSize, { 0, 1 }, { 1, 0 });

        ImGui::End();
        ImGui::PopStyleVar();
    }
}


void ChessApplication::OnWindowClose() {
    m_Running = false;
}

void ChessApplication::OnKeyPressed(int32_t key, int32_t scancode, int32_t action, int32_t mods) {
    if (key == GLFW_KEY_ESCAPE) {
        m_Running = false;
    }
}

void ChessApplication::OnMouseButton(int32_t button, int32_t action, int32_t mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        double x, y;
        glfwGetCursorPos(m_Window, &x, &y);

        x = (x - m_WindowProperties.Width / 2) / (m_WindowProperties.Width / 2) * 8.0;
        y = (y - m_WindowProperties.Height / 2) / (m_WindowProperties.Height / 2) * -4.5;

        if (action == GLFW_PRESS) {
            m_IsHoldingPiece = true;

            if (x > -4 && x < 4 && y > -4 && y < 4) {
                Square rank = (Square)(x + 4.0);
                Square file = (Square)(y + 4.0);

                // The square the mouse clicked on
                Square selectedSquare = ToSquare('a' + rank, '1' + file);

                // If a piece was already selected, move piece to clicked square
                if (m_SelectedPiece != INVALID_SQUARE && m_SelectedPiece != selectedSquare) {
                    if (m_LegalMoves & (1ull << selectedSquare) || selectedSquare == m_SelectedPiece) {
                        m_Board.Move({ m_SelectedPiece, selectedSquare });
                        m_BoardFEN = m_Board.ToFEN();
                    }

                    m_SelectedPiece = INVALID_SQUARE;
                    m_LegalMoves = 0;
                }
                else {  // If no piece already selected, select piece
                    m_LegalMoves = m_Board.GetLegalMoves(selectedSquare);
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
                Square rank = (Square)(x + 4.0);
                Square file = (Square)(y + 4.0);

                // The square the mouse was released on
                Square selectedSquare = ToSquare('a' + rank, '1' + file);

                if (m_SelectedPiece != INVALID_SQUARE) {
                    if (m_LegalMoves & (1ull << selectedSquare)) {
                        m_Board.Move({ m_SelectedPiece, selectedSquare });
                        m_BoardFEN = m_Board.ToFEN();
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

