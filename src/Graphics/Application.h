#pragma once

#include "Chess/Board.h"

#include <string>

#include <glm/glm.hpp>

struct GLFWwindow;

class Application {
public:
    Application(uint32_t width, uint32_t height, const std::string& name);
    Application(const Application&) = delete;
    Application(Application&&) = delete;

    virtual ~Application();

    Application& operator=(const Application&) = delete;
    Application& operator=(Application&&) = delete;

    static Application& Get() { return *s_Instance; }

    void Run();

    virtual void OnInit() = 0;
    virtual void OnRender() = 0;
    virtual void RenderImGui() = 0;
private:
    static Application* s_Instance;
protected:
    virtual void OnWindowClose() = 0;
    virtual void OnKeyPressed(int32_t key, int32_t scancode, int32_t action, int32_t mods) = 0;
    virtual void OnMouseButton(int32_t button, int32_t action, int32_t mods) = 0;
protected:
    GLFWwindow* m_Window = nullptr;

    struct {
        uint32_t Width, Height;
        std::string Name;
    } m_WindowProperties;

    bool m_Running = false;
};
