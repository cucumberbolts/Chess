#pragma once

#include <string>

struct GLFWwindow;

class Application {
public:
    Application(uint32_t width, uint32_t height, const std::string& name);
    Application(const Application& other) = delete;

    ~Application();

    static Application& Get() { return *s_Instance; }

    void Run();
private:
    static Application* s_Instance;
private:
    void OnWindowClose();
    void OnKeyPressed(int32_t key, int32_t scancode, int32_t action, int32_t mods);
private:
    GLFWwindow* m_Window = nullptr;

    struct {
        uint32_t Width, Height;
        std::string Name;
    } m_WindowProperties;

    bool m_Running = false;
};
