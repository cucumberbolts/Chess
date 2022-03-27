#include <iostream>

#include <ChessEngine/Engine.h>

#include <Graphics/Application.h>

static uint32_t s_Allocations = 0;
static uint32_t s_BytesAllocated = 0;
static uint32_t s_Frees = 0;

void* operator new(size_t size) {
    s_Allocations++;
    s_BytesAllocated += size;
    return malloc(size);
}

void operator delete(void* ptr) noexcept {
    s_Frees++;
    free(ptr);
}

int main() {
#if 1
    Application* app = new Application(1280, 720, "Application");
    app->Run();
    delete app;
#elif 0
    std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;

    std::string_view stockfish14 = "Engines/stockfish_14_x64_avx2.exe";
    std::string_view leelaChess0 = "Engines/lc0.exe";

    auto engine = Engine::Create(leelaChess0);

    engine->Init();

    std::cout << "Printing info...\n";
    engine->PrintInfo();
    std::cout << "Done printing info...\n";

    std::cout << "Press enter to start engine:\n";
    std::cin.get();

    engine->Run();
    std::cin.get();

    std::cout << "Stopping engine...\n";
    engine->Stop();

    std::cout << "Best continuation: ";
    for (AlgebraicMove m : engine->m_BestContinuation.Continuation)
        std::cout << m << " ";

    std::cout << "\nDepth: " << engine->m_BestContinuation.Depth << "\n\n";

    std::cout << "Board: \n" << engine->m_Board << "\n";

    std::cout << s_Allocations << " allocations!\n";

    std::cin.get();
#else
    std::string_view stockfish = "Engines/stockfish_14_x64_avx2.exe";

    {
        std::unique_ptr<Engine> engine = Engine::Create(stockfish);

        engine->Init();

        engine->Run();
        std::cout << "Running engine!\n";
        std::cin.get();
        std::cout << "Stopping engine!\n";
    }

    std::cout << s_Allocations << " allocations!\n";
    std::cout << s_BytesAllocated << " bytes allocated!\n";
    std::cout << s_Frees << " frees!\n";
#endif
}
