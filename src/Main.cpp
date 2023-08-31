#include "ChessApplication.h"

#ifdef _WIN32

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    auto app = new ChessApplication(1600, 900, "Chess");
    try {
        app->Run();
    }
    catch (std::exception& e) {
        MessageBox(glfwGetWin32Window(app->GetGLFWWindow()), e.what(), NULL, MB_OK | MB_ICONERROR);
    }
    delete app;

    return 0;
}

#else

#include <iostream>

int main() {
    auto app = new ChessApplication(1280, 720, "Chess");
    try {
        app->Run();
    }
    catch (std::exception& e) {
        std::cout << "Unhandled exception: " << e.what() << "\n";
        std::cin.get();
    }
    delete app;
}

#endif
