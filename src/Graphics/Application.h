#pragma once

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
private:
    GLFWwindow* m_Window = nullptr;

    bool m_Running = false;
};
