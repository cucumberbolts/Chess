#include "Application.h"

#include <iostream>

Application* Application::s_Instance = nullptr;

Application::Application() {
    if (!s_Instance)
        s_Instance = this;

    if (!glfwInit()) {
        std::cout << "Could not initialize GLFW!\n";
        return;
    }

    m_Window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!m_Window) {
        std::cout << "Could not create window!\n";
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_Window);

    glfwSetWindowUserPointer(m_Window, this);

    glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
    {
        Application* app = (Application*)glfwGetWindowUserPointer(window);
        app->OnWindowClose();
    });
}

Application::~Application() {
    glfwTerminate();
}

void Application::Run() {
    m_Running = true;

    while (m_Running) {
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(m_Window);

        glfwPollEvents();
    }
}

void Application::OnWindowClose() {
    m_Running = false;

    std::cout << "Closing window!\n";
}
