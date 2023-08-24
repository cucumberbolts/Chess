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

#include <sstream>

static glm::vec4 HexToColour (uint32_t colour)
{
    static constexpr float normalise = 1.0f / 255.0f;
    uint8_t r = (colour & 0xff000000) >> 24;
    uint8_t g = (colour & 0x00ff0000) >> 16;
    uint8_t b = (colour & 0x0000ff00) >> 8;
    uint8_t a = (colour & 0x000000ff);
    return glm::vec4{ r, g, b, a } * normalise;
}

ChessApplication::ChessApplication (uint32_t width, uint32_t height, const std::string &name)
    : Application (width, height, name), m_ChessViewportSize (width, height)
{
}

void ChessApplication::OnInit()
{
    Renderer::Init (glm::ortho (-8.0f, 8.0f, -4.5f, 4.5f));

    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImFontConfig fontConfig;
    fontConfig.FontDataOwnedByAtlas = false;

    void *font = (void *)Resources::Fonts::Roboto::ROBOTO_REGULAR;
    int32_t fontSize = sizeof (Resources::Fonts::Roboto::ROBOTO_REGULAR);
    io.FontDefault = io.Fonts->AddFontFromMemoryTTF (font, fontSize, 20.0f, &fontConfig);

    //font = (void*)Resources::Fonts::Roboto::ROBOTO_BOLD;
    //fontSize = sizeof(Resources::Fonts::Roboto::ROBOTO_BOLD);
    //io.Fonts->AddFontFromMemoryTTF(font, fontSize, 20.0f, &fontConfig);

    if (!std::filesystem::exists ("imgui.ini"))
        ImGui::LoadIniSettingsFromMemory (Resources::DEFAULT_IMGUI_INI);

    ImGui::StyleColorsDark();

    ImGuiStyle &style = ImGui::GetStyle();
    style.WindowMenuButtonPosition = ImGuiDir_None;
    style.GrabRounding = 4.0f;
    style.WindowRounding = 4.0f;
    style.FrameRounding = 4.0f;
    style.WindowBorderSize = 0.0f;
    style.PopupBorderSize = 0.0f;
    style.ChildBorderSize = 0.0f;
    style.WindowMinSize = { 200.0f, 200.0f };

    std::shared_ptr<Texture> chessPieces = std::make_shared<Texture> (Resources::Textures::CHESS_PIECES, sizeof (Resources::Textures::CHESS_PIECES));

    const float tileSize = (float)chessPieces->GetWidth() / 6.0f;
    for (int y = 0; y < 2; y++)
        for (int x = 0; x < 6; x++)
            m_ChessPieceSprites[y * 6 + x] = std::make_shared<SubTexture> (chessPieces, glm::vec2 (x, y), glm::vec2 (tileSize));

    m_DarkSquareColour = HexToColour (0x532A00FF);
    m_LightSquareColour = HexToColour (0xFFB160FF);
    m_LegalMoveColour = { 1.0f, 0.0f, 1.0f, 0.5f };
    m_BackgroundColour = { 0.2f, 0.2f, 0.2f, 1.0f };

    m_BoardFEN = Board::StartFen;
    m_BoardFEN.resize (100);

    FramebufferSpecification spec;
    spec.Width = m_WindowProperties.Width;
    spec.Height = m_WindowProperties.Height;
    m_ChessViewport = std::make_shared<Framebuffer> (spec);
}

std::shared_ptr<SubTexture> ChessApplication::GetChessSprite (Piece p)
{
    switch (p) {
    case WhitePawn:
        return m_ChessPieceSprites[11];
    case WhiteKnight:
        return m_ChessPieceSprites[9];
    case WhiteBishop:
        return m_ChessPieceSprites[8];
    case WhiteRook:
        return m_ChessPieceSprites[10];
    case WhiteQueen:
        return m_ChessPieceSprites[7];
    case WhiteKing:
        return m_ChessPieceSprites[6];
    case BlackPawn:
        return m_ChessPieceSprites[5];
    case BlackKnight:
        return m_ChessPieceSprites[3];
    case BlackBishop:
        return m_ChessPieceSprites[2];
    case BlackRook:
        return m_ChessPieceSprites[4];
    case BlackQueen:
        return m_ChessPieceSprites[1];
    case BlackKing:
        return m_ChessPieceSprites[0];
    case None:
        return nullptr;
    }
}

void ChessApplication::OnRender()
{
    Renderer::ClearScreen ({ 0.0f, 0.0f, 0.0f, 1.0f });

    // Resize framebuffer according to viewport size
    FramebufferSpecification spec = m_ChessViewport->GetSpecification();
    if ((float)spec.Width != m_ChessViewportSize.x || (float)spec.Height != m_ChessViewportSize.y) {
        m_ChessViewport->Resize ((uint32_t)m_ChessViewportSize.x, (uint32_t)m_ChessViewportSize.y);

        // I don't know what to call this variable
        constexpr float temp = 4.5f;

        float aspectRatio = m_ChessViewportSize.x / m_ChessViewportSize.y;

        // Resize the coordinates so the chessboard size
        // is the minimumm of the width and the height
        if (aspectRatio <= 1.0f)
            Renderer::UpdateProjectionMatrix (glm::ortho (-temp, temp, -temp / aspectRatio, temp / aspectRatio));
        else
            Renderer::UpdateProjectionMatrix (glm::ortho (-temp * aspectRatio, temp * aspectRatio, -temp, temp));

        // Transform the ImGui mouse coordinates to OpenGL coordinates
        m_CoordinateTransform = glm::inverse (Renderer::GetProjectionMatrix());
        m_CoordinateTransform = glm::scale (m_CoordinateTransform, { 1 / (m_ChessViewportSize.x * 0.5f), 1 / (m_ChessViewportSize.y * -0.5f), 1.0f });
        m_CoordinateTransform = glm::translate (m_CoordinateTransform, { m_ChessViewportSize.x * -0.5f, m_ChessViewportSize.y * -0.5f, 0.0f });
    }

    // Render chessboard to framebuffer
    m_ChessViewport->Bind();
    Renderer::ClearScreen (m_BackgroundColour);

    // Draw chess board
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            const glm::vec4 colour = (x + y) % 2 == 0 ? m_LightSquareColour : m_DarkSquareColour;
            Renderer::DrawRect ({ -3.5f + x, 3.5f - y, 0.0f }, { 1.0f, 1.0f }, colour);
        }
    }

    // Draw legal moves (loop through m_LegalMoves bitboard)
    for (BitBoard legalMoves = m_LegalMoves; legalMoves; legalMoves &= legalMoves - 1) {
        Square square = GetSquare (legalMoves);
        int rank = RankOf (square);
        int file = FileOf (square);
        Renderer::DrawRect ({ -3.5f + file, -3.5f + rank, 0.0f }, { 1.0f, 1.0f }, m_LegalMoveColour);
    }

    // Draw pieces
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            std::shared_ptr<SubTexture> piece = GetChessSprite (m_Board[y * 8 + x]);

            if (piece && (y * 8 + x) != m_SelectedPiece)
                Renderer::DrawRect ({ -3.5f + x, -3.5f + y, 0.0f }, { 1.0f, 1.0f }, piece);
        }
    }

    // Draw selected piece following the mouse
    if (m_SelectedPiece != INVALID_SQUARE)
        Renderer::DrawRect ({ m_BoardMousePosition.x, m_BoardMousePosition.y, 0.5f }, { 1, 1 }, GetChessSprite (m_Board[m_SelectedPiece]));

    Renderer::Flush();

    // Render ImGui to window
    m_ChessViewport->Unbind();
}

void ChessApplication::RenderImGui()
{
    static bool s_ShowColoursWindow = true, s_ShowFENWindow = true, s_ShowEngineWindow = true;

    {
        // Fullscreen stuff
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos (viewport->WorkPos);
        ImGui::SetNextWindowSize (viewport->WorkSize);
        ImGui::SetNextWindowViewport (viewport->ID);
        ImGui::PushStyleVar (ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar (ImGuiStyleVar_WindowBorderSize, 0.0f);

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
        ImGui::PushStyleVar (ImGuiStyleVar_WindowPadding, ImVec2 (0.0f, 0.0f));
        ImGui::Begin ("DockSpace Demo", nullptr, window_flags);
        ImGui::PopStyleVar();

        ImGui::PopStyleVar (2);

        // Submit the DockSpace
        static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;
        ImGuiID dockspaceID = ImGui::GetID ("MyDockSpace");
        ImGui::DockSpace (dockspaceID, ImVec2 (0.0f, 0.0f), dockspaceFlags);

        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu ("File")) {
                ImGui::MenuItem ("New");

                ImGui::Separator();

                if (ImGui::MenuItem ("Quit"))
                    m_Running = false;

                ImGui::EndMenu();
            } else if (ImGui::BeginMenu ("View")) {
                if (ImGui::MenuItem ("Colours")) { s_ShowColoursWindow = true; }
                if (ImGui::MenuItem ("FEN"))     { s_ShowFENWindow = true; }
                if (ImGui::MenuItem ("Engine"))  { s_ShowEngineWindow = true; }

                ImGui::EndMenu();
            } else if (ImGui::BeginMenu ("About")) {
                ImGui::Text ("OpenGL Version: %s", Renderer::GetOpenGLVersion());

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        ImGui::End();
    }

    if (s_ShowColoursWindow) {
        ImGuiColorEditFlags colourEditFlags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar;

        ImGui::Begin ("Colours", &s_ShowColoursWindow);

        static int currentTheme;
        static const char *themes[] = { "Dark", "Light" };

        if (ImGui::BeginTable ("ColoursTable", 2, ImGuiTableFlags_SizingFixedSame)) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text ("Theme");
            ImGui::TableNextColumn();
            if (ImGui::Combo ("##Theme", &currentTheme, themes, sizeof (themes) / sizeof (const char *))) {
                if (currentTheme == 0)
                    ImGui::StyleColorsDark();
                else
                    ImGui::StyleColorsLight();
            }

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text ("Background colour");
            ImGui::TableNextColumn();
            ImGui::ColorEdit4 ("##Background colour", glm::value_ptr (m_BackgroundColour), colourEditFlags);

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text ("Dark square colour");
            ImGui::TableNextColumn();
            ImGui::ColorEdit4 ("##Dark square colour", glm::value_ptr (m_DarkSquareColour), colourEditFlags);

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text ("Light square colour");
            ImGui::TableNextColumn();
            ImGui::ColorEdit4 ("##Light square colour", glm::value_ptr (m_LightSquareColour), colourEditFlags);

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text ("Legal move colour");
            ImGui::TableNextColumn();
            ImGui::ColorEdit4 ("##Legal move colour", glm::value_ptr (m_LegalMoveColour), colourEditFlags);

            ImGui::EndTable();
        }

        ImGui::End();
    }

    if (s_ShowFENWindow) {
        ImGui::Begin ("FEN", &s_ShowFENWindow);

        if (ImGui::InputText ("##FEN", m_BoardFEN.data(), m_BoardFEN.size(), ImGuiInputTextFlags_EnterReturnsTrue))
            m_Board.FromFEN (m_BoardFEN);

        if (ImGui::Button ("Copy FEN to clipboard"))
            glfwSetClipboardString (m_Window, m_BoardFEN.c_str());

        if (ImGui::Button ("Reset board")) {
            m_Board.Reset();  // Reset FEN string
            m_BoardFEN = Board::StartFen;
            if (m_RunningEngine)
                m_RunningEngine->SetPosition (m_BoardFEN);
        }

        ImGui::End();
    }

    {
        ImGui::PushStyleVar (ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
        ImGui::PushStyleVar (ImGuiStyleVar_WindowMinSize, { 400.f, 400.f }); // For when window is floating

        ImGui::Begin ("Chessboard");

        // Get the position of the viewport relative to the panel position
        // (The tab bar pushes it down)
        auto [offsetX, offsetY] = ImGui::GetCursorPos();

        ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        m_ChessViewportSize = { viewportSize.x, viewportSize.y };

        ImTextureID texture = (void *) (intptr_t)m_ChessViewport->GetColourAttachment();
        int framePadding = 0;
        ImVec4 backgroundColour = ImVec4 (0.0f, 0.0f, 0.0f, 1.0f);
        ImVec4 tintColour = ImVec4 (1.0f, 1.0f, 1.0f, 1.0f);
        // Using ImageButton instead of Image allows the viewport to
        // not be dragged when it is floating when dragging the pieces
        ImGui::ImageButton (texture, viewportSize, { 0, 1 }, { 1, 0 }, framePadding, backgroundColour, tintColour);

        // Get mouse position on the board
        auto [mouseX, mouseY] = ImGui::GetMousePos();     // Absolute mouse position
        auto [windowX, windowY] = ImGui::GetWindowPos();  // Top-Left corner of window

        glm::vec2 mousePosition = { mouseX, mouseY };
        glm::vec2 viewportPosition = { windowX + offsetX, windowY + offsetY };
        glm::vec2 relativeMousePos = mousePosition - viewportPosition;

        m_BoardMousePosition = m_CoordinateTransform * glm::vec4{ relativeMousePos.x, relativeMousePos.y, 1.0f, 1.0f };

        ImGui::End();

        ImGui::PopStyleVar (2);
    }

    if (s_ShowEngineWindow) {
        ImGui::Begin ("Engine", &s_ShowEngineWindow);

        static auto s_SelectedEngine = m_Engines.end();

        if (s_SelectedEngine == m_Engines.end()) {
            if (ImGui::Button ("Create engine")) {
                ImGui::OpenPopup ("Create engine");
            }

#define GetCentre GetCenter

            // Always centre this window when appearing
            ImVec2 centre = ImGui::GetMainViewport()->GetCentre();
            ImGui::SetNextWindowPos (centre, ImGuiCond_Appearing, ImVec2 (0.5f, 0.5f));

            if (ImGui::BeginPopupModal ("Create engine", nullptr, ImGuiWindowFlags_None)) {
                static char path[255] = { 0 };
                static char name[64] = { 0 };

                ImGui::InputText ("path", path, sizeof (path));
                if (ImGui::Button ("Select executable")) {
                    FileDialog::Open (path, sizeof (path));

                    // If the 'name' field is empty, copy the executable name to it
                    if (name[0] == '\0')
                        strncpy (name, std::filesystem::path (path).stem().u8string().c_str(),  sizeof (name));
                }

                ImGui::InputText ("name", name, sizeof (name));

                if (ImGui::Button ("OK", ImVec2 (120, 0))) {
                    m_Engines.emplace_back (std::pair{ name, path });
                    s_SelectedEngine = m_Engines.end();

                    // "Reset" the strings for next time popup is opened
                    path[0] = '\0';
                    name[0] = '\0';
                    ImGui::CloseCurrentPopup();
                }

                ImGui::SameLine();

                if (ImGui::Button ("Cancel", ImVec2 (120, 0))) {
                    // "Reset" the strings for next time popup is opened
                    path[0] = '\0';
                    name[0] = '\0';
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }

            for (auto it = m_Engines.begin(); it != m_Engines.end();) {
                const std::string &name = it->first;
                const std::filesystem::path &path = it->second;

                ImGui::Separator();

                ImGui::Text ("%s", name.c_str());

                ImGui::PushID ((int)std::distance (m_Engines.begin(), it));

                if (ImGui::Button ("Start engine")) {
                    ImGui::PopID();

                    try {
                        m_RunningEngine = Engine::Create (path);
                        m_RunningEngine->SetUpdateCallback ([this] (const Engine::BestContinuation & c) { OnEngineUpdate (c); });
                        m_RunningEngine->Init();
                        m_RunningEngine->SetPosition (m_BoardFEN);
                        m_RunningEngine->Run();

                        s_SelectedEngine = it;
                    } catch (EngineCreationFailure &) {
                        ImGui::OpenPopup ("Failed to create engine");
                    } catch (EngineNotReady &) {
                        ImGui::OpenPopup ("Engine not ready");
                    }
                } else {
                    ImGui::PopID();
                }

                if (ImGui::Button ("Delete engine")) {
                    it = m_Engines.erase (it);
                    s_SelectedEngine = m_Engines.end();
                } else {
                    ++it;
                }
            }

            ImGui::Separator();
        } else {
            if (m_RunningEngine->GetThreadException() != nullptr) {
                std::rethrow_exception (m_RunningEngine->GetThreadException());
            }

            ImGui::Text ("%s", s_SelectedEngine->first.c_str());

            if (ImGui::Button ("Stop engine")) {
                m_RunningEngine->Stop();  // Stop the engine
                m_RunningEngine.reset();  // Stop the process
                s_SelectedEngine = m_Engines.end();
            } else {
                ImGui::Text ("Depth: %i", m_BestContinuation.Depth);

                if (!m_BestContinuation.Mate) {
                    float score = (float)m_BestContinuation.Score * 0.01f;
                    if (m_Board.GetPlayerTurn() == Black) score *= -1.0f;

                    ImGui::Text ("Score: %.2f", score);
                } else {
                    const char *text = (m_Board.GetPlayerTurn() == White) ? "Score: M%i" : "Score: -M%i";
                    ImGui::Text (text, m_BestContinuation.Score);
                }

                ImGui::Text ("%s", m_BestContinuationAlgebraicMoves.c_str());
                if (ImGui::Button ("Copy to clipboard"))
                    ImGui::SetClipboardText (m_BestContinuationAlgebraicMoves.c_str());
            }
        }

        // TODO: ImGui Error message wrapper?
        ImVec2 centre = ImGui::GetMainViewport()->GetCentre();
        ImGui::SetNextWindowPos (centre, ImGuiCond_Appearing, ImVec2 (0.5f, 0.5f));

        if (ImGui::BeginPopupModal ("Failed to create engine")) {
            if (ImGui::Button ("OK"))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }

        centre = ImGui::GetMainViewport()->GetCentre();
        ImGui::SetNextWindowPos (centre, ImGuiCond_Appearing, ImVec2 (0.5f, 0.5f));

        if (ImGui::BeginPopupModal ("Engine not ready")) {
            if (ImGui::Button ("OK"))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }

        ImGui::End();
    }
}

void ChessApplication::OnWindowClose()
{
    m_Running = false;
}

void ChessApplication::OnWindowResize (int32_t width, int32_t height)
{
    m_WindowProperties.Width = width;
    m_WindowProperties.Height = height;
}

void ChessApplication::OnKeyPressed (int32_t key, int32_t scancode, int32_t action, int32_t mods)
{
    if (key == GLFW_KEY_ESCAPE) {
        m_Running = false;
    }
}

void ChessApplication::OnMouseButton (int32_t button, int32_t action, int32_t mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        // Convert ImGui viewport coordinates to rendering coordinates
        glm::vec2 &point = m_BoardMousePosition;

        if (action == GLFW_PRESS) {
            m_IsHoldingPiece = true;

            if (point.x > -4 && point.x < 4 && point.y > -4 && point.y < 4) {
                Square rank = (Square) (point.x + 4.0f);
                Square file = (Square) (point.y + 4.0f);

                // The square the mouse clicked on
                Square selectedSquare = ToSquare ('a' + rank, '1' + file);

                // If a piece was already selected, move piece to clicked square
                if (m_SelectedPiece != INVALID_SQUARE && m_SelectedPiece != selectedSquare) {
                    if (m_LegalMoves & (1ull << selectedSquare) || selectedSquare == m_SelectedPiece) {
                        m_Board.Move ({ m_SelectedPiece, selectedSquare });
                        m_BoardFEN = m_Board.ToFEN();
                        if (m_RunningEngine)
                            m_RunningEngine->SetPosition (m_BoardFEN);
                    }

                    m_SelectedPiece = INVALID_SQUARE;
                    m_LegalMoves = 0;
                } else { // If no piece already selected, select piece
                    m_LegalMoves = m_Board.GetPieceLegalMoves (selectedSquare);
                    m_SelectedPiece = m_LegalMoves == 0 ? INVALID_SQUARE : selectedSquare;
                }
            } else {
                m_SelectedPiece = INVALID_SQUARE;
                m_LegalMoves = 0;
            }
        } else if (action == GLFW_RELEASE) {
            if (point.x > -4 && point.x < 4 && point.y > -4 && point.y < 4) {
                Square rank = (Square) (point.x + 4.0f);
                Square file = (Square) (point.y + 4.0f);

                // The square the mouse was released on
                Square selectedSquare = ToSquare ('a' + rank, '1' + file);

                if (m_SelectedPiece != INVALID_SQUARE) {
                    if (m_LegalMoves & (1ull << selectedSquare)) {
                        m_Board.Move ({ m_SelectedPiece, selectedSquare });
                        m_BoardFEN = m_Board.ToFEN();
                        if (m_RunningEngine)
                            m_RunningEngine->SetPosition (m_BoardFEN);
                        m_LegalMoves = 0;
                    }
                }
            }

            m_IsHoldingPiece = false;
            m_SelectedPiece = INVALID_SQUARE;
        }
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        m_SelectedPiece = INVALID_SQUARE;
        m_IsHoldingPiece = false;
        m_LegalMoves = 0;
    }
}

void ChessApplication::OnEngineUpdate (const Engine::BestContinuation &bestContinuation)
{
    m_BestContinuation = bestContinuation;

    Board moveTranslator (m_Board);

    std::ostringstream continuationText;
    for (LongAlgebraicMove m : m_BestContinuation.Continuation)
        continuationText << moveTranslator.Move (m) << " ";

    m_BestContinuationAlgebraicMoves = continuationText.str();
}
