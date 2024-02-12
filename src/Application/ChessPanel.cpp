#include "Application.h"

#include "Graphics/Renderer.h"

#include <imgui.h>

#include <glm/gtc/matrix_transform.hpp>

void Application::RenderBoard() {
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

        // Transform the ImGui mouse coordinates to OpenGL coordinates
        m_CoordinateTransform = glm::inverse(Renderer::GetProjectionMatrix());
        m_CoordinateTransform = glm::scale(m_CoordinateTransform, { 1 / (m_ChessViewportSize.x * 0.5f), 1 / (m_ChessViewportSize.y * -0.5f), 1.0f });
        m_CoordinateTransform = glm::translate(m_CoordinateTransform, { m_ChessViewportSize.x * -0.5f, m_ChessViewportSize.y * -0.5f, 0.0f });
    }

    // Render chessboard to framebuffer
    m_ChessViewport->Bind();
    Renderer::ClearScreen(m_BackgroundColour);

    // Draw chess board
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            const glm::vec4 colour = (x + y) % 2 == 0 ? m_LightSquareColour : m_DarkSquareColour;
            Renderer::DrawRect({ -3.5f + x, 3.5f - y, 0.0f }, { 1.0f, 1.0f }, colour);
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
            std::shared_ptr<SubTexture> piece = GetChessSprite(m_Board[y * 8 + x]);

            if (piece && (y * 8 + x) != m_SelectedPiece)
                Renderer::DrawRect({ -3.5f + x, -3.5f + y, 0.0f }, { 1.0f, 1.0f }, piece);
        }
    }

    // Draw selected piece following the mouse
    if (m_SelectedPiece != INVALID_SQUARE)
        Renderer::DrawRect({ m_BoardMousePosition.x, m_BoardMousePosition.y, 0.5f }, { 1, 1 }, GetChessSprite(m_Board[m_SelectedPiece]));

    Renderer::Flush();

    // Deselect framebuffer to render ImGui to window
    m_ChessViewport->Unbind();
}

void Application::RenderChessPanel() {
    RenderBoard();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, { 400.f, 400.f });  // For when window is floating

    ImGui::Begin("Chessboard");

    // Get the position of the viewport relative to the panel position
    // (The tab bar pushes it down)
    auto [offsetX, offsetY] = ImGui::GetCursorPos();

    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
    m_ChessViewportSize = { viewportSize.x, viewportSize.y };

    ImTextureID texture = (void*)(intptr_t)m_ChessViewport->GetColourAttachment();
    int framePadding = 0;
    ImVec4 backgroundColour = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    ImVec4 tintColour = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    // Using ImageButton instead of Image allows the viewport to
    // not be dragged when it is floating when dragging the pieces
    ImGui::ImageButton(texture, viewportSize, { 0, 1 }, { 1, 0 }, framePadding, backgroundColour, tintColour);

    // Get mouse position on the board
    auto [mouseX, mouseY] = ImGui::GetMousePos();     // Absolute mouse position
    auto [windowX, windowY] = ImGui::GetWindowPos();  // Top-Left corner of window

    glm::vec2 mousePosition = { mouseX, mouseY };
    glm::vec2 viewportPosition = { windowX + offsetX, windowY + offsetY };
    glm::vec2 relativeMousePos = mousePosition - viewportPosition;

    m_BoardMousePosition = m_CoordinateTransform * glm::vec4{ relativeMousePos.x, relativeMousePos.y, 1.0f, 1.0f };

    ImGui::End();

    ImGui::PopStyleVar(2);
}
