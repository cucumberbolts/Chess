#include "Chess/Board.h"
#include "Engine/Engine.h"

#include <iostream>

void EngineCallback(const Engine::BestContinuation& continuation) {
    auto best = continuation;

    Board moveTranslator;

    std::cout << "Best continutation: ";

    for (LongAlgebraicMove m : best.Continuation)
        std::cout << moveTranslator.Move(m) << " ";

    std::cout << "\n";
}

int main() {
    std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;

    // Changes according to the working directory
    std::string_view stockfish14 = "../../Engines/stockfish_14_x64_avx2.exe";
    std::string_view leelaChess0 = "../../Engines/lc0.exe";

    auto engine = Engine::Create(leelaChess0);
    engine->SetUpdateCallback(EngineCallback);
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

    std::cout << "Press enter to exit the test\n";
    std::cin.get();
}
