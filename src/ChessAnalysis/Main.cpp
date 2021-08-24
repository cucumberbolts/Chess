#include <Chess/Engine.h>

#include <iostream>

int main() {
    const char* stockfish14 = "stockfish_14_x64_avx2.exe";
    const char* leelaChess0 = "lc0.exe";

    Engine* engine = Engine::Create(stockfish14);
    engine->Init();

    engine->PrintOptions();

    delete engine;

    std::cin.get();
}
