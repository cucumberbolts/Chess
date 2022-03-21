#include "Graphics/Application.h"

int main() {
    Application* app = new Application();
    app->Run();
    delete app;
}
