#pragma once

#include "Chess/Board.h"

#include <string>

struct GLFWwindow;

class Application {
public:
    Application(uint32_t width, uint32_t height, const std::string& name);
    Application(const Application&) = delete;
    Application(Application&&) = delete;

    ~Application();

    Application& operator=(const Application&) = delete;
    Application& operator=(Application&&) = delete;

    static Application& Get() { return *s_Instance; }

    void Run();
private:
    static Application* s_Instance;
private:
    void OnWindowClose();
    void OnKeyPressed(int32_t key, int32_t scancode, int32_t action, int32_t mods);
    void OnMouseButton(int32_t button, int32_t action, int32_t mods);
private:
    GLFWwindow* m_Window = nullptr;

    struct {
        uint32_t Width, Height;
        std::string Name;
    } m_WindowProperties;

    bool m_Running = false;

    Board m_Board;
    Square m_SelectedPiece = INVALID_SQUARE;
    bool m_IsHoldingPiece = false;  // If the selected piece follows the mouse
    BitBoard m_LegalMoves = 0;
    std::string m_BoardFEN;
};
