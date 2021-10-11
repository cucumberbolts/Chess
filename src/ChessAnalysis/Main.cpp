#include <Chess/Engine.h>

#include <iostream>

static uint32_t s_Allocations = 0;
void* operator new(size_t size) {
    s_Allocations++;
    return malloc(size);
}

int main() {
    std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;

    std::string_view stockfish14 = "Engines/stockfish_14_x64_avx2.exe";
    std::string_view leelaChess0 = "Engines/lc0.exe";

    auto engine = Engine::Create(stockfish14);

    engine->Init();

    std::cout << "Printing info...\n";
    engine->PrintInfo();
    std::cout << "Done printing info...\n";

    engine->Run();

    std::cin.get();

    std::cout << "Stopping engine...\n";
    engine->Stop();

    std::cout << s_Allocations << " allocations!\n";

    std::cin.get();
}
