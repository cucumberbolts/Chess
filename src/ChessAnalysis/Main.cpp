#include <Chess/Engine.h>

#include <iostream>

static uint32_t s_Allocations = 0;
void* operator new(size_t size) {
    s_Allocations++;
    return malloc(size);
}

int main() {
    const char* stockfish14 = "stockfish_14_x64_avx2.exe";

    Engine* engine = Engine::Create(stockfish14);
    engine->Init();

    engine->PrintInfo();

    delete engine;

    std::cout << s_Allocations << " allocations!\n";

    std::cin.get();
}
