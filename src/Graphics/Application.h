#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class Application {
public:
    Application();
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

    bool m_Running = false;
};
