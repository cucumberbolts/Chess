#include <Chess/Engine.h>

#include <iostream>

static uint32_t s_Allocations = 0;
void* operator new(size_t size) {
    s_Allocations++;
    return malloc(size);
}

int main() {
    std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;

    const char* stockfish14 = "C:/Users/julian/Workspace/Chess-Analysis/Engines/lc0.exe";

    auto engine = Engine::Create(stockfish14);

    engine->Init();

    std::cout << "Printing info...\n";
    engine->PrintInfo();
    std::cout << "Done printing info...\n";

    std::cout << s_Allocations << " allocations!\n";

    std::cin.get();
}
